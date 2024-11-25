#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <fstream>
#include <unordered_set>

// 枚举类型定义：状态枚举
enum class State {
    Initial,      // 初始状态
    Id,           // 标识符状态
    GT,           // '>' 状态
    GE,           // '>=' 状态
    LT,           // '<' 状态
    LE,           // '<=' 状态
    EQ,           // '=' 状态
    EQEQ,         // '==' 状态
    IntLiteral,   // 整数状态
    FloatLiteral, // 浮点数状态
    Comment,      //忽略一整行的状态
    Slash         //检测到第一个'/'后的状态
};

// 枚举类型定义：Token类型
enum class TokenType {
    Identifier,    // 标识符
    Keyword,       // 关键字
    GT,            // '>'
    GE,            // '>='
    LT,            // '<'
    LE,            // '<='
    EQ,            // '='
    EQEQ,          // '=='
    IntLiteral,    // 整数字面量
    FloatLiteral,  // 浮点数字面量
    Slash          //单独的'/'
};

// Token 结构体
struct Token {
    TokenType type;     // Token 类型
    std::string text;   // Token 文本
};

// 关键字集合
const std::unordered_set<std::string> keywords = {
    "if", "else", "while", "return", "int", "void", "float"
};

// 初始化并识别 token
State initToken(char ch, std::vector<Token>& tokens, std::string& tokenText, Token& token) {
    if (!tokenText.empty()) {
        // 如果 Token 是标识符，检查是否为关键字
        if (token.type == TokenType::Identifier) {
            if (keywords.find(tokenText) != keywords.end()) {
                token.type = TokenType::Keyword;
            }
        }
        // 将 Token 加入结果列表
        token.text = tokenText;
        tokens.push_back(token);
        tokenText.clear();
    }

    // 根据当前字符更新状态
    if (isalpha(ch)) {
        token.type = TokenType::Identifier;
        tokenText.push_back(ch);
        return State::Id;
    } else if (isdigit(ch)) {
        token.type = TokenType::IntLiteral;
        tokenText.push_back(ch);
        return State::IntLiteral;
    } else if (ch == '>') {
        token.type = TokenType::GT;
        tokenText.push_back(ch);
        return State::GT;
    } else if (ch == '<') {
        token.type = TokenType::LT;
        tokenText.push_back(ch);
        return State::LT;
    } else if (ch == '=') {
        token.type = TokenType::EQ;
        tokenText.push_back(ch);
        return State::EQ;
    } else if (ch == '#') {
        return State::Comment;
    } else if (ch == '/') {
        token.type = TokenType::Slash;
        tokenText.push_back(ch);
        return State::Slash;
    }

    // 返回初始状态
    return State::Initial;
}

// 词法分析器
std::vector<Token> tokenize(const std::string& code) {
    std::vector<Token> tokens;      // 存储结果 Token
    std::string tokenText;          // 当前 Token 文本
    Token token;                    // 当前 Token
    State state = State::Initial;   // 当前状态

    for (char ch : code) {
        switch (state) {
            case State::Initial:
                state = initToken(ch, tokens, tokenText, token);
                break;

            case State::Id:
                if (isalpha(ch) || isdigit(ch)) {
                    tokenText.push_back(ch);
                } else {
                    state = initToken(ch, tokens, tokenText, token);
                }
                break;

            case State::GT:
                if (ch == '=') {
                    token.type = TokenType::GE;
                    tokenText.push_back(ch);
                    state = State::GE;
                } else {
                    state = initToken(ch, tokens, tokenText, token);
                }
                break;

            case State::LT:
                if (ch == '=') {
                    token.type = TokenType::LE;
                    tokenText.push_back(ch);
                    state = State::LE;
                } else {
                    state = initToken(ch, tokens, tokenText, token);
                }
                break;

            case State::EQ:
                if (ch == '=') {
                    token.type = TokenType::EQEQ;
                    tokenText.push_back(ch);
                    state = State::EQEQ;
                } else {
                    state = initToken(ch, tokens, tokenText, token);
                }
                break;

            case State::GE:
            case State::LE:
            case State::EQEQ:
                state = initToken(ch, tokens, tokenText, token);
                break;

            case State::IntLiteral:
                if (isdigit(ch)) {
                    tokenText.push_back(ch);
                } else if (ch == '.') {
                    tokenText.push_back(ch);
                    state = State::FloatLiteral;
                } else {
                    state = initToken(ch, tokens, tokenText, token);
                }
                break;

            case State::FloatLiteral:
                if (isdigit(ch)) {
                    tokenText.push_back(ch);
                } else {
                    state = initToken(ch, tokens, tokenText, token);
                }
                break;
            case State::Comment:
                if (ch == '\n') {
                    state = State::Initial;
                }
                break;
            case State::Slash:
                if (ch=='/') {
                    //确认是注释,忽略整行
                    tokenText.clear();
                    state = State::Comment;
                } else {
                    //单独的一个'/',初始化新Token
                    state = initToken(ch, tokens, tokenText, token);
                }
                break;
        }
    }

    // 处理最后一个 token
    if (!tokenText.empty()) {
        if (token.type == TokenType::Identifier && keywords.find(tokenText) != keywords.end()) {
            token.type = TokenType::Keyword;
        }
        token.text = tokenText;
        tokens.push_back(token);
    }

    return tokens;
}

// 测试代码
int main() {
    std::ifstream inputFile("source.cpp"); // 从文件读取
    if (!inputFile.is_open()) {
        std::cerr << "open error" << std::endl;
        return 1;
    }

    std::string code;
    std::string line;
    while (std::getline(inputFile, line)) {
        code += line + '\n'; // 将每一行添加到 code 中
    }

    inputFile.close(); // 关闭文件

    auto tokens = tokenize(code);

    for (const auto& token : tokens) {
        std::cout << "Token type: " << static_cast<int>(token.type)
                  << ", text: " << token.text << std::endl;
    }

    return 0;
}
