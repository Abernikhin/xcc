
#ifndef __TokenTypes__
#define __TokenTypes__

enum TokenType {

    Token_EOF,
    
    Token_Storage,
    Token_Modifier,
    Token_Type,
    Token_Pointer,
    Token_Keyword,

    Token_Assign,

    Token_Plus,
    Token_Minus,
    Token_Star,
    Token_Divide,

    Token_Semicolon,
    Token_Comma,

    Token_Lparent,
    Token_Rparent,
    Token_Begin,
    Token_End,

    Token_Id,
    Token_Number,

    Token_Declaration,
    Token_Argumant,
    Token_Function,
    Token_Call,
    Token_Body,

};

#endif // __TokenTypes__
