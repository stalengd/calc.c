#include "parser.h"

ParsingResult buildBlocksTree(Vector tokens, BlockNode* resultPointer) {
    ParsingResult result;

    BlockNode root = {
        .members = vectorCreate(4, sizeof(BlockMember)),
        .stringStart = ((TokenNode*)vectorGet(&tokens, 0))->str
    };
    BlockNode* currentBlock = &root;
    
    Vector bracketsStack = vectorCreate(8, sizeof(BlockNode*));

    size_t i;
    for (i = 0; i < tokens.size; i++)
    {
        TokenNode* token = vectorGet(&tokens, i);
        switch (token->type)
        {
        case TOKEN_OPEN_BRACKET: {
            BlockMember newMember = {
                .type = TOKEN_OPEN_BRACKET,
                .data.block = {
                    .members = vectorCreate(4, sizeof(BlockMember)),
                    .stringStart = token->str
                }
            };
            vectorPush(&currentBlock->members, &newMember);
            vectorPush(&bracketsStack, &currentBlock);
            currentBlock = &((BlockMember*)vectorLast(&currentBlock->members))->data.block;
            break;
        }
        case TOKEN_CLOSE_BRACKET: {
            Vector prevBlockMembers = currentBlock->members;

            if (vectorPop(&bracketsStack, &currentBlock) == false) {
                result.isError = true;
                result.errorPosition = token->str;
                result.errorMessage = "You are closing more brackets than opening";
                goto end;
            }

            if (prevBlockMembers.size == 0) {
                result.isError = true;
                result.errorPosition = token->str;
                result.errorMessage = "Block is empty";
                goto end;
            }
            if (prevBlockMembers.size == 1) {
                void* member = vectorGet(&prevBlockMembers, 0);
                vectorPop(&currentBlock->members, NULL);
                vectorPush(&currentBlock->members, member);
                vectorFree(&prevBlockMembers);
            }

            break;
        }
        default: {
            BlockMember newMember = {
                .type = token->type,
                .data.token = *token
            };
            vectorPush(&currentBlock->members, &newMember);
            break;
        }
        }
    }

    if (bracketsStack.size != 0) {
        result.isError = true;
        result.errorPosition = PARSING_ERROR_POSITION_END;
        result.errorMessage = "You are opening more brackets than closing";
        goto end;
    }

    if (root.members.size == 1) {
        BlockMember* member = (BlockMember*)vectorGet(&root.members, 0);
        if (member->type == TOKEN_OPEN_BRACKET) {
            Vector oldVector = root.members;
            root.members = member->data.block.members;
            vectorFree(&oldVector);
        }
    }

    *resultPointer = root;
    result.isError = false;


    end:
    if (result.isError) {
        freeBlocksTree(root);
    }
    vectorFree(&bracketsStack); 
    return result;
}

ParsingResult buildExpressionTree(BlockNode block, ExpressionNode** resultPointer) {
    ExpressionNode* expRoot = malloc(sizeof(ExpressionNode));
    expRoot->type = TOKEN_NONE;
    expRoot->left = NULL;
    expRoot->right = NULL;

    ExpressionNode* node = expRoot;
    int tokenIndex = block.members.size - 1;
    BlockMember* token = vectorGet(&block.members, tokenIndex);
    while (token != NULL)
    {
        switch (token->type)
        {
        case TOKEN_NUMBER: {
            LiteralNode* numberNode 
                = createLiteralNode(strtof(token->data.token.str, NULL));
            if (node->right == NULL) {
                node->right = (ExpressionNode*)numberNode;
            }
            else if (node->left == NULL) {
                if (node->type == TOKEN_NONE) {
                    free(numberNode);
                    ParsingResult result = {
                        .isError = true,
                        .errorPosition = token->data.token.str,
                        .errorMessage = "Several values in a row are not supported"
                    };
                    freeExpressionTree(expRoot);
                    return result;
                }
                node->left = (ExpressionNode*)numberNode;
            }
            else {
                free(numberNode);
                ParsingResult result = {
                    .isError = true,
                    .errorPosition = token->data.token.str,
                    .errorMessage = "Several values in a row are not supported"
                };
                freeExpressionTree(expRoot);
                return result;
            }
            break;
        }
        case TOKEN_PLUS:
        case TOKEN_MINUS:
        case TOKEN_MULTIPLY:
        case TOKEN_DIVIDE:
        {
            ParsingResult result = appendOperatorToTree(node, token, &node);
            if (result.isError) {
                return result;
            }
            break;
        }
        case TOKEN_OPEN_BRACKET: {
            ExpressionNode* innerNode = malloc(sizeof(ExpressionNode));
            innerNode->type = TOKEN_OPEN_BRACKET;
            innerNode->left = (ExpressionNode*)token;
            if (node->right == NULL) {
                node->right = innerNode;
            }
            else {
                node->left = innerNode;
            }
            break;
        }

        default:
            // Not supported
            break;
        }

        tokenIndex--;
        token = vectorGet(&block.members, tokenIndex);
    }

    if (node->left == NULL) {
        if (node->type == TOKEN_MINUS) {
            node->left = (ExpressionNode*)createLiteralNode(0);
        }
        else if (node->type == TOKEN_NONE) {
            ExpressionNode* right = node->right;
            free(node);
            freeExpressionTree(expRoot);
            *resultPointer = right;
            ParsingResult result = {
                .isError = false
            };
            return result;
        }
        else {
            BlockMember* startMember = (BlockMember*)vectorGet(&block.members, 0);
            char* pos = startMember->type == TOKEN_OPEN_BRACKET ? 
                startMember->data.block.stringStart : 
                startMember->data.token.str;
            ParsingResult result = {
                .isError = true,
                .errorPosition = pos,
                .errorMessage = "Expected value at start of the block"
            };
            freeExpressionTree(expRoot);
            return result;
        }
    }

    expRoot = sortExpressionTree(expRoot);

    ParsingResult result = replaceBlockWithExpression(expRoot);
    if (result.isError) {
        freeExpressionTree(expRoot);
        return result;
    }

    *resultPointer = expRoot;
    result.isError = false;
    return result;
}

LiteralNode* createLiteralNode(float value) {
    LiteralNode* numberNode = malloc(sizeof(LiteralNode));
    numberNode->type = TOKEN_NUMBER;
    numberNode->number = value;
    return numberNode;
}

ParsingResult appendOperatorToTree(ExpressionNode* node, BlockMember* token, ExpressionNode** resultPointer) {
    if (node->right == NULL) {
        ParsingResult result = {
            .isError = true,
            .errorPosition = token->data.token.str + 2,
            .errorMessage = "Expected value after operator"
        };
        return result;
    }
    if (node->left == NULL) {
        if (node->type == TOKEN_MINUS) {
            node->type = TOKEN_NONE;
            ExpressionNode* newNode = malloc(sizeof(ExpressionNode));
            newNode->type = TOKEN_MINUS;
            newNode->left = (ExpressionNode*)createLiteralNode(0);
            newNode->right = node->right;
            node->right = newNode;
        }
        if (node->type == TOKEN_NONE) {
            node->type = token->type;
        }
        else {
            ParsingResult result = {
                .isError = true,
                .errorPosition = token->data.token.str,
                .errorMessage = "Several incompatible operators in a row"
            };
            return result;
        }
    }
    else {
        ExpressionNode* newNode = malloc(sizeof(ExpressionNode));
        newNode->type = token->type;
        newNode->left = NULL;
        newNode->right = node->left;
        node->left = newNode;
        node = newNode;
    }
    *resultPointer = node;
    ParsingResult result = {
        .isError = false
    };
    return result;
}

ParsingResult replaceBlockWithExpression(ExpressionNode* node) {
    if (node->left->type == TOKEN_OPEN_BRACKET) {
        BlockMember* block = (BlockMember*)node->left->left;
        void* old = node->left;
        ParsingResult result = buildExpressionTree(block->data.block, &node->left);
        free(old);
        if (result.isError) {
            return result;
        }
    }
    else if (isOperation(node->left->type)) {
        ParsingResult result = replaceBlockWithExpression(node->left);
        if (result.isError) {
            return result;
        }
    }
    if (node->right->type == TOKEN_OPEN_BRACKET) {
        BlockMember* block = (BlockMember*)node->right->left;
        void* old = node->right;
        ParsingResult result = buildExpressionTree(block->data.block, &node->right);
        free(old);
        if (result.isError) {
            return result;
        }
    }
    else if (isOperation(node->right->type)){
        ParsingResult result = replaceBlockWithExpression(node->right);
        if (result.isError) {
            return result;
        }
    }

    ParsingResult result = {
        .isError = false
    };
    return result;
}

ExpressionNode* sortExpressionTree(ExpressionNode* expRoot) {
    bool sorted = false;
    while (!sorted)
    {
        sorted = true;
        ExpressionNode* node = expRoot;
        ExpressionNode* prev = NULL;
        int curPriority = getOperationPriority(node->type);
        while (isOperation(node->left->type)) {
            ExpressionNode* next = node->left;
            int nextPriority = getOperationPriority(next->type);
            if (nextPriority < curPriority) {
                sorted = false;
                node->left = next->right;
                next->right = node;
                if (node == expRoot)
                    expRoot = next;
                if (prev != NULL)
                    prev->left = next;
                prev = next;
            }
            else {
                prev = node;
            }
            node = next;
            curPriority = nextPriority;
        }
    }

    return expRoot;
}

int getOperationPriority(enum TokenType type) {
    switch (type)
    {
    case TOKEN_PLUS:
    case TOKEN_MINUS: return 0;
    case TOKEN_MULTIPLY:
    case TOKEN_DIVIDE: return 1;
    default: return 255;
    }
}

bool isOperation(enum TokenType type) {
    switch (type)
    {
    case TOKEN_PLUS:
    case TOKEN_MINUS:
    case TOKEN_MULTIPLY:
    case TOKEN_DIVIDE: return true;
    default: return false;
    }
}

void freeBlocksTree(BlockNode node) {
    if (node.members.data == NULL) return;
    for (size_t i = 0; i < node.members.size; i++)
    {
        BlockMember* m = vectorGet(&node.members, i);
        if (m->type == TOKEN_OPEN_BRACKET) {
            freeBlocksTree(m->data.block);
        }
    }
    vectorFree(&node.members);
}

void freeExpressionTree(ExpressionNode* node) {
    if (node == NULL) return;
    if (isOperation(node->type)) {
        freeExpressionTree(node->left);
        freeExpressionTree(node->right);
    }
    free(node);
}