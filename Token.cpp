#include "Token.h"

static void split(std::vector<std::string>& buffer, const std::string& txt) {
    int offset = 0;

    while (1) {
        int pos = txt.find(" ", offset);
        if (pos == std::string::npos) break;

        buffer.push_back(txt.substr(offset, pos - offset));
        offset = pos+1;
    }
    buffer.push_back(txt.substr(offset, txt.size() - offset));
}

/*Tokenizes string of html tokens. Provide empty wrapper token. Works recursively on parts.
Returns text of higher level token, with all internal tokens removed.**/
std::string tokenize(Token* tk, const std::string& text)
{
    //exit condition
    if (text.size() == 0) return "";
    if (text.find("<", 0) == std::string::npos) return text;

    std::string parent_text = text;
    while (1) {
        int head_start = parent_text.find("<", 0);
        if (head_start == std::string::npos) return parent_text;
        int head_end = parent_text.find(">", head_start);
        if (head_end == std::string::npos) return parent_text; //html broken

        std::vector<std::string> args;
        std::string tmp = parent_text.substr(head_start + 1, head_end - head_start - 1);
        split(args, tmp);

        //find tail
        int tail_start;
        int tail_end;
        bool no_tail = false;
        if (parent_text.find("<" + args.front(), head_end + 1) == std::string::npos) {    //no same tag
            tail_start = parent_text.find("</" + args.front(), head_end + 1);
            if (tail_start == std::string::npos) no_tail = true;
            if (!no_tail) {
                tail_end = parent_text.find(">", tail_start + 1);
                if (tail_end == std::string::npos) return parent_text; //html broken
            }
        }
        else {  // same tag exists
            //count number of occurences of same tag
            int count = 0;
            const char* chars = parent_text.c_str();
            const int end_index = parent_text.size() - 1 - 1 - args.front().size();

            for (int index = 0; index < end_index; index++) {
                if (chars[index] != '<') continue;
                if (chars[index + 1] == '/') {
                    if (strncmp(chars + index + 1 + 1, args.front().c_str(), args.front().size()) == 0) count--;
                }
                else {
                    if (strncmp(chars + index + 1, args.front().c_str(), args.front().size()) == 0) count++;
                }
                if (count == 0) {
                    no_tail = false;
                    tail_start = index;
                    tail_end = parent_text.find(">", tail_start + 1);
                    if (tail_end == std::string::npos) return parent_text; //html broken
                    break;
                }
            }
            if (count != 0) no_tail = true;
        }

        //create token
        Token* t = new Token(args.front());
        for (int i = 1; i < args.size(); i++) {
            std::string& str = args[i];
            int pos = str.find("=", 0);
            if (pos != std::string::npos) {
                t->options.insert(std::pair<std::string, std::string>(str.substr(0, pos), str.substr(pos + 1, str.size() - pos - 1)));
            }
            else {
                t->options.insert(std::pair<std::string, std::string>(str, ""));
            }
        }
        //need to add args
        tk->children.push_back(t);

        if (no_tail) {
            t->text = "";
            parent_text = parent_text.substr(head_end + 1, parent_text.size() - head_end - 1);
        }
        else {
            std::string rest = tokenize(t, parent_text.substr(head_end + 1, tail_start - head_end - 1));
            t->text = rest;

            //prepare parent text
            parent_text = parent_text.substr(0, head_start) + parent_text.substr(tail_end + 1, parent_text.size() - tail_end - 1);
        }       
    }

    return parent_text;
}

void find_links(Token& tk, std::vector<std::string>& buffer) {
    buffer.push_back(tk.options["href"]);
    std::for_each(tk.children.begin(), tk.children.end(), [&](Token *t) {find_links(*t, buffer);});
}

void release(Token* tk)
{
    std::for_each(tk->children.begin(), tk->children.end(), [](Token* t) {release(t); });
    delete tk;
}
