#include "parser.h"

BlockNode buildBlocksTree(Vector tokens) {
    BlockNode root = {
        .members = vectorCreate(4, sizeof(BlockMember))
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
                    .members = vectorCreate(4, sizeof(BlockMember))
                }
            };
            vectorPush(&currentBlock->members, &newMember);
            vectorPush(&bracketsStack, &currentBlock);
            currentBlock = &((BlockMember*)vectorLast(&currentBlock->members))->data.block;
            break;
        }
        case TOKEN_CLOSE_BRACKET: {
            if (vectorPop(&bracketsStack, &currentBlock) == false) {
                // TODO: error
                printf("ERROR: you are closing more brackets than opening\n");
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
        // TODO: error
        printf("ERROR: you are opening %d more brackets than closing\n", bracketsStack.size);
    }

    vectorFree(&bracketsStack);
    
    return root;
}

ExpressionNode* buildExpressionTree(BlockNode block) {
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
            else {
                node->left = (ExpressionNode*)numberNode;
            }
            break;
        }
        case TOKEN_PLUS:
        case TOKEN_MINUS:
        case TOKEN_MULTIPLY:
        case TOKEN_DIVIDE:
        {
            node = appendOperatorToTree(node, token->type);
            break;
        }
        case TOKEN_OPEN_BRACKET: {
            ExpressionNode* innerNode = malloc(sizeof(ExpressionNode)); //buildExpressionTree(token->data.block);
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
        else {
            // TODO: error
            printf("ERROR: expected value at start of the block\n");
        }
    }

    expRoot = sortExpressionTree(expRoot);

    replaceBlockWithExpression(expRoot);

    return expRoot;
}

LiteralNode* createLiteralNode(float value) {
    LiteralNode* numberNode = malloc(sizeof(LiteralNode));
    numberNode->type = TOKEN_NUMBER;
    numberNode->number = value;
    return numberNode;
}

ExpressionNode* appendOperatorToTree(ExpressionNode* node, enum TokenType type) {
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
            node->type = type;
        }
        else {
            // TODO: error
            printf("ERROR: several incompatible operators in a row\n");
        }
    }
    else {
        ExpressionNode* newNode = malloc(sizeof(ExpressionNode));
        newNode->type = type;
        newNode->left = NULL;
        newNode->right = node->left;
        node->left = newNode;
        node = newNode;
    }
    return node;
}

void replaceBlockWithExpression(ExpressionNode* node) {
    if (node->left->type == TOKEN_OPEN_BRACKET) {
        BlockMember* block = (BlockMember*)node->left->left;
        void* old = node->left;
        node->left = buildExpressionTree(block->data.block);
        free(old);
    }
    else if (isOperation(node->left->type)) {
        replaceBlockWithExpression(node->left);
    }
    if (node->right->type == TOKEN_OPEN_BRACKET) {
        BlockMember* block = (BlockMember*)node->right->left;
        void* old = node->right;
        node->right = buildExpressionTree(block->data.block);
        free(old);
    }
    else if (isOperation(node->right->type)){
        replaceBlockWithExpression(node->right);
    }
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