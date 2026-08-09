
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
    Token_AddAssign,
    Token_SubAssign,
    Token_MulAssign,
    Token_DivAssign,
    Token_ModAssign,
    Token_AndAssign,
    Token_OrAssign,
    Token_XorAssign,
    Token_LeftShiftAssign,
    Token_RightShiftAssign,

    Token_Plus,
    Token_Minus,
    Token_Star,
    Token_Divide,
    Token_Modulo,
    Token_Equal,
    Token_NotEqual,
    Token_Less,
    Token_Greater,
    Token_LessEqual,
    Token_GreaterEqual,
    Token_LogicalAnd,
    Token_LogicalOr,
    Token_LogicalNot,
    Token_BitAnd,
    Token_BitOr,
    Token_BitXor,
    Token_BitNot,
    Token_LeftShift,
    Token_RightShift,
    Token_Inc,
    Token_Dec,

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
