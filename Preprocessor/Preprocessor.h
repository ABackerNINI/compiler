//
// Created by abacker on 2/25/17.
//

#ifndef PREPROCESSOR_PREPROCESSOR_H
#define PREPROCESSOR_PREPROCESSOR_H

#include <cstdio>
#include <cstdlib>
#include <list>
#include <cstring>

namespace compiler {

    struct _str {
        const char *p;
        size_t len;
    };

    enum _sym {
        ALIGNAS,
        ALIGNOF,
        AND,
        AND_EQ,
        ASM,
        ATOMIC_CANCEL,
        ATOMIC_COMMIT,
        ATOMIC_NOEXCEPT,
        AUTO,
        BITAND,
        BITOR,
        BOOL,
        BREAK,
        CASE,
        CATCH,
        CHAR,
        CHAR16_T,
        CHAR32_T,
        CLASS,
        COMPL,
        CONCEPT,
        CONST,
        CONSTEXPR,
        CONST_CAST,
        CONTINUE,
        DECLTYPE,
        DEFAULT,
        DELETE,
        DO,
        DOUBLE,
        DYNAMIC_CAST,
        ELSE,
        ENUM,
        EXPLICIT,
        EXPORT,
        EXTERN,
        FALSE,
        FLOAT,
        FOR,
        FRIEND,
        GOTO,
        IF,
        IMPORT,
        INLINE,
        INT,
        LONG,
        MODULE,
        MUTABLE,
        NAMESPACE,
        NEW,
        NOEXCEPT,
        NOT,
        NOT_EQ,
        NULLPTR,
        OPERATOR,
        OR,
        OR_EQ,
        PRIVATE,
        PROTECTED,
        PUBLIC,
        REGISTER,
        REINTERPRET_CAST,
        REQUIRES,
        RETURN,
        SHORT,
        SIGNED,
        SIZEOF,
        STATIC,
        STATIC_ASSERT,
        STATIC_CAST,
        STRUCT,
        SWITCH,
        SYNCHRONIZED,
        TEMPLATE,
        THIS,
        THREAD_LOCAL,
        THROW,
        TRUE,
        TRY,
        TYPEDEF,
        TYPEID,
        TYPENAME,
        UNION,
        UNSIGNED,
        USING,
        VIRTUAL,
        VOID,
        VOLATILE,
        WCHAR_T,
        WHILE,
        XOR,
        XOR_EQ
    };

    struct _elem {
        _str str;
        _sym sym;
    };

    const char *_skip(const char *s, const char *end, const char *delims, struct _str *v);

    inline bool isdigit(const char c) { return c >= '0' && c <= '9'; }

    inline bool isletter(const char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }

    bool strcmp(const char *s1, const char *s2, size_t n);

    bool strcmp(const _str *s1, const _str *s2);

    void strprintf(const _str *str);

    char *ReadFile(const char *_Path);

    namespace preprocessor {
        static const char *_Spliter = "()[]{}<>:,.;~!%^*-+|&=";
        static const char *_Spliter0 = "\n\t\r\v ";
        static const char *_Spliter1 = "()[]{}<>:,.;";
        static const char *_Spliter2 = "~!%^*-+|";
        static const char *_Spliter3 = "&=";
        static const char *_Spliter4[2] = {
                "#include", "#define"
        };

        static const _str keywords[] = {
                {"#include", 8},
                {"#define",  7}
        };

        inline bool iswhitespace(const char c) {
            for (const char *p = _Spliter0; *p; ++p) {
                if (*p == c)return true;
            }
            return false;
        }

        inline bool isspliter(const char c) {
            for (const char *p = _Spliter; *p; ++p) {
                if (*p == c)return true;
            }
            return false;
        }

        inline bool isboundery(const char c) {
            for (const char *p = _Spliter1; *p; ++p) {
                if (*p == c)return true;
            }
            return false;
        }

        inline size_t isusrdefname(const char *s) {
            size_t isudn = 0;
            for (; *s; ++s) {
                if (isletter(*s) || isdigit(*s) || *s == '_') {
                    ++isudn;
                } else {
                    break;
                }
            }
            return isudn;
        }

        inline size_t iskeyword(const char *s) {
            for (int i = 0; i < 2; ++i) {
                if (strcmp(s, keywords[i].p, keywords[i].len)) {
                    return keywords[i].len;
                }
            }
            return 0;
        }

        const char *_get(const char *s, _str *v);

        void DealIncludeAndDefine(std::list<_str> *list);

        std::list<_str> ParseCpp(const char *s);

        void Preprocess(const char *_Path);
    }
}

#endif //PREPROCESSOR_PREPROCESSOR_H
