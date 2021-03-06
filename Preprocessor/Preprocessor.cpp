//
// Created by abacker on 2/25/17.
//

#include <vector>
#include <map>
#include "Preprocessor.h"

using namespace compiler;

const char *compiler::_skip(const char *s, const char *end, const char *delims, struct _str *v) {
    v->p = s;
    while (s < end && strchr(delims, *(unsigned char *) s) == NULL) s++;
    v->len = s - v->p;
    while (s < end && strchr(delims, *(unsigned char *) s) != NULL) s++;
    return s;
}

void compiler::strprintf(const _str *str) {
    for (const char *p = str->p, *end = p + str->len; p < end; ++p) {
        printf("%c", *p);
    }
    fflush(stdout);
}

bool compiler::strcmp(const char *s1, const char *s2, size_t n) {
    for (size_t i = 0; i < n; ++i, ++s1, ++s2) {
        if (*s1 == '\0' || *s2 == '\0' || *s1 != *s2)return false;
    }
    return true;
}

bool compiler::strcmp(const _str *s1, const _str *s2) {
    if (s1->len == s2->len) {
        for (size_t i = 0; i < s1->len; ++i) {
            if (*(s1->p + i) != *(s2->p + i))return false;
        }
    } else return false;

    return true;
}

char *compiler::ReadFile(const char *_Path) {
    FILE *fp = fopen(_Path, "rb");
    if (fp == NULL) {
        return NULL;
    }

    long file_size;
    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *content = (char *) malloc((file_size + 1) * sizeof(char));
    if (content == NULL) {
        fclose(fp);
        return NULL;
    }

    fread(content, (size_t) file_size, sizeof(char), fp);
    content[file_size] = '\0';
    fclose(fp);

    return content;
}

const char *compiler::preprocessor::_get(const char *s, _str *v) {
    for (; *s && iswhitespace(*s); ++s) {
    }

    v->p = s;

    if (isspliter(*s)) {
        v->len = 1;
        return s + 1;
    } else if (*s == '\"') {
        v->len = 2;
        for (++s; *s; ++s) {
            if (*s != '\"') {
                ++v->len;
            } else {
                return s + 1;
            }
        }
        //mark:throw no match quote
    } else if (*s == '/' && *(s + 1) == '/') {
        for (; *s && *s != '\n'; ++s);
        return _get(s, v);
    } else if (v->len = iskeyword(s), v->len > 0) {
        return s + v->len;
    } else if (v->len = isusrdefname(s), v->len > 0) {
        return s + v->len;
    } else {
        for (; *s; ++s) {
            if (!iswhitespace(*s)) {
                ++v->len;
            } else {
                return s + 1;
            }
        }
        //mark
    }
    return NULL;
}

std::list<_str> compiler::preprocessor::ParseCpp(const char *s) {
    std::list<_str> list;
    _str str;

    while (s = _get(s, &str), s)list.push_back(str);

    for (auto iter = list.begin(); iter != list.end(); ++iter) {
        strprintf(&*iter);
        printf("|");
    }

    return list;
}

void compiler::preprocessor::DealIncludeAndDefine(std::list<_str> *list) {
    std::map<_str *, _str *> map_define;
    for (auto iter = list->begin(); iter != list->end(); ++iter) {
        if (strcmp("#include", iter->p, iter->len)) {
            iter = list->erase(iter);
            auto path = iter;
            if (path->p[0] == '\"' && path->p[path->len - 1] == '\"') {
                char *_path = (char *) malloc(path->len * sizeof(char));
                memcpy(_path, path->p + 1, path->len);
                _path[path->len - 2] = '\0';

                const char *c = ReadFile(_path);////mark:delete
                if (c) {
                    path->p = c;
                    path->len = strlen(c);
//                  printf("%s\n", c);
                }
            }
        } else if (strcmp("#define", iter->p, iter->len)) {
            auto def_name = ++iter;
            auto def_value = ++iter;

            map_define[&*def_name] = &*def_value;
        } else {
            for (auto iter_md = map_define.begin(); iter_md != map_define.end(); ++iter_md) {
                if (strcmp(&*iter, &*iter_md->first)) {
                    iter->p = iter_md->second->p;
                    iter->len = iter_md->second->len;
                }
            }
        }
    }

    for (auto iter = list->begin(); iter != list->end(); ++iter) {
        strprintf(&*iter);
        printf("|");
    }
    printf("\n");
}

void compiler::preprocessor::Preprocess(const char *_Path) {
    char *content = ReadFile(_Path);
    if (content == NULL) {
        //todo
    } else {
//    printf("%s", content);

        auto list = ParseCpp(content);

        DealIncludeAndDefine(&list);

        free(content);
    }
}