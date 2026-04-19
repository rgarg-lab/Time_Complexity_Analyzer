#include <bits/stdc++.h>
using namespace std;

// ==================== TOKENIZER ====================

enum TokenType { 
    KEYWORD, IDENTIFIER, NUMBER, OPERATOR, PUNCTUATION, 
    STRING_LITERAL, COMMENT, END_OF_FILE, UNKNOWN 
};

struct Token {
    TokenType type;
    string value;
    int line;
};

class Tokenizer {
    string code;
    size_t pos = 0;
    int line = 1;
    
    set<string> keywords = {
        "for", "while", "do", "if", "else", "switch", "case", "default",
        "return", "break", "continue", "goto", "int", "void", "double",
        "float", "char", "bool", "long", "short", "unsigned", "signed",
        "struct", "class", "enum", "typedef", "const", "static", "extern",
        "new", "delete", "sizeof", "true", "false", "nullptr", "auto"
    };
    
    void skipWhitespace() {
        while (pos < code.size() && isspace(code[pos])) {
            if (code[pos] == '\n') line++;
            pos++;
        }
    }
    
    bool match(const string& s) {
        if (pos + s.size() <= code.size() && code.substr(pos, s.size()) == s) {
            pos += s.size();
            return true;
        }
        return false;
    }
    
public:
    Tokenizer(const string& src) : code(src) {}
    
    Token next() {
        skipWhitespace();
        if (pos >= code.size()) return {END_OF_FILE, "", line};
        
        int startLine = line;
        
        // Single-line comment
        if (match("//")) {
            while (pos < code.size() && code[pos] != '\n') pos++;
            return next(); // skip comment
        }
        
        // Multi-line comment
        if (match("/*")) {
            while (pos + 1 < code.size() && !(code[pos] == '*' && code[pos+1] == '/')) {
                if (code[pos] == '\n') line++;
                pos++;
            }
            pos += 2;
            return next(); // skip comment
        }
        
        // String literal
        if (code[pos] == '"') {
            pos++;
            string val = "\"";
            while (pos < code.size() && code[pos] != '"') {
                if (code[pos] == '\\' && pos + 1 < code.size()) {
                    val += code[pos++];
                }
                val += code[pos++];
            }
            if (pos < code.size()) pos++;
            val += "\"";
            return {STRING_LITERAL, val, startLine};
        }
        
        // Character literal
        if (code[pos] == '\'') {
            pos++;
            string val = "'";
            while (pos < code.size() && code[pos] != '\'') {
                if (code[pos] == '\\' && pos + 1 < code.size()) {
                    val += code[pos++];
                }
                val += code[pos++];
            }
            if (pos < code.size()) pos++;
            val += "'";
            return {STRING_LITERAL, val, startLine};
        }
        
        // Number
        if (isdigit(code[pos])) {
            string val;
            while (pos < code.size() && (isalnum(code[pos]) || code[pos] == '.')) {
                val += code[pos++];
            }
            return {NUMBER, val, startLine};
        }
        
        // Identifier or keyword
        if (isalpha(code[pos]) || code[pos] == '_') {
            string val;
            while (pos < code.size() && (isalnum(code[pos]) || code[pos] == '_')) {
                val += code[pos++];
            }
            if (keywords.count(val)) return {KEYWORD, val, startLine};
            return {IDENTIFIER, val, startLine};
        }
        
        // Multi-character operators
        vector<string> multiOps = {
            "<<=", ">>=", "...", "==", "!=", "<=", ">=", "&&", "||",
            "++", "--", "+=", "-=", "*=", "/=", "%=", "&=", "|=", 
            "^=", "<<", ">>", "::", "->", ".*", "->*"
        };
        for (const auto& op : multiOps) {
            if (match(op)) return {OPERATOR, op, startLine};
        }
        
        // Single-character operator or punctuation
        char c = code[pos++];
        string val(1, c);
        if (string("+-*/%&|^~!<>=?:").find(c) != string::npos) {
            return {OPERATOR, val, startLine};
        }
        return {PUNCTUATION, val, startLine};
    }
    
    vector<Token> tokenize() {
        vector<Token> tokens;
        Token t;
        while ((t = next()).type != END_OF_FILE) {
            tokens.push_back(t);
        }
        return tokens;
    }
};

// ==================== COMPLEXITY REPRESENTATION ====================

struct Complexity {
    int polynomial = 0;      // n^polynomial
    int logarithmic = 0;     // (log n)^logarithmic
    int exponential = 0;     // 2^n, etc.
    int factorial = 0;       // n!
    bool unknown = false;
    
    static Complexity constant() { return {0, 0, 0, 0, false}; }
    static Complexity linear() { return {1, 0, 0, 0, false}; }
    static Complexity logn() { return {0, 1, 0, 0, false}; }
    static Complexity nlogn() { return {1, 1, 0, 0, false}; }
    static Complexity quadratic() { return {2, 0, 0, 0, false}; }
    static Complexity exponentialC() { return {0, 0, 1, 0, false}; }
    
    Complexity operator*(const Complexity& other) const {
        if (unknown || other.unknown) return {0, 0, 0, 0, true};
        return {
            polynomial + other.polynomial,
            logarithmic + other.logarithmic,
            exponential + other.exponential,
            factorial + other.factorial,
            false
        };
    }
    
    Complexity max(const Complexity& other) const {
        if (unknown) return other;
        if (other.unknown) return *this;
        
        // Compare by dominance: factorial > exponential > polynomial > logarithmic
        if (factorial != other.factorial) 
            return factorial > other.factorial ? *this : other;
        if (exponential != other.exponential)
            return exponential > other.exponential ? *this : other;
        if (polynomial != other.polynomial)
            return polynomial > other.polynomial ? *this : other;
        if (logarithmic != other.logarithmic)
            return logarithmic > other.logarithmic ? *this : other;
        return *this;
    }
    
    string toString() const {
        if (unknown) return "O(?)";
        if (factorial > 0) return "O(n!)";
        if (exponential > 0) return "O(2^n)";
        
        if (polynomial == 0 && logarithmic == 0) return "O(1)";
        
        string result = "O(";
        bool needSpace = false;
        
        if (polynomial > 0) {
            if (polynomial == 1) result += "n";
            else result += "n^" + to_string(polynomial);
            needSpace = true;
        }
        
        if (logarithmic > 0) {
            if (needSpace) result += " ";
            if (logarithmic == 1) result += "log n";
            else result += "log^" + to_string(logarithmic) + " n";
        }
        
        result += ")";
        return result;
    }
};

// ==================== FUNCTION INFO ====================

struct FunctionInfo {
    string name;
    string returnType;
    vector<pair<string, string>> parameters; // type, name
    int startToken;
    int endToken;
    int bodyStart;
    int bodyEnd;
    bool isRecursive = false;
    int recursiveCalls = 0;
    Complexity complexity;
};

// ==================== ANALYZER ====================

class ComplexityAnalyzer {
    vector<Token> tokens;
    map<string, FunctionInfo> functions;
    int pos = 0;
    
    Token peek(int offset = 0) {
        int idx = pos + offset;
        if (idx < 0 || idx >= tokens.size()) return {END_OF_FILE, "", 0};
        return tokens[idx];
    }
    
    Token advance() {
        if (pos < tokens.size()) return tokens[pos++];
        return {END_OF_FILE, "", 0};
    }
    
    bool check(TokenType type, const string& value = "") {
        if (peek().type != type) return false;
        if (!value.empty() && peek().value != value) return false;
        return true;
    }
    
    bool match(TokenType type, const string& value = "") {
        if (check(type, value)) { advance(); return true; }
        return false;
    }
    
    // Find matching close brace and return end position
    int findMatchingBrace(int start) {
        int depth = 1;
        int i = start + 1;
        while (i < tokens.size() && depth > 0) {
            if (tokens[i].value == "{") depth++;
            else if (tokens[i].value == "}") depth--;
            i++;
        }
        return i - 1;
    }
    
    // Find matching parenthesis
    int findMatchingParen(int start) {
        int depth = 1;
        int i = start + 1;
        while (i < tokens.size() && depth > 0) {
            if (tokens[i].value == "(") depth++;
            else if (tokens[i].value == ")") depth--;
            i++;
        }
        return i - 1;
    }
    
    // ==================== LOOP ANALYSIS ====================
    
    struct LoopInfo {
        string type;
        Complexity iterationComplexity;
        bool isLogarithmic;
    };
    
    LoopInfo analyzeLoopComplexity(int start, int end) {
        LoopInfo info = {"for", Complexity::linear(), false};
        
        // Look for update expression patterns
        for (int i = start; i <= end; i++) {
            string val = tokens[i].value;
            
            // Multiplicative update → O(log n)
            if (val == "*=" || val == "/=" || val == "*" || val == "/") {
                info.isLogarithmic = true;
                info.iterationComplexity = Complexity::logn();
            }
            // Bit shift → O(log n)
            if (val == "<<=" || val == ">>=" || val == "<<" || val == ">>") {
                info.isLogarithmic = true;
                info.iterationComplexity = Complexity::logn();
            }
        }
        
        return info;
    }
    
    // ==================== BLOCK ANALYSIS ====================
    
    Complexity analyzeBlock(int start, int end, const string& currentFunc) {
        if (start > end) return Complexity::constant();
        
        Complexity total = Complexity::constant();
        int i = start;
        
        while (i <= end) {
            Token t = tokens[i];
            
            // For loop
            if (t.type == KEYWORD && t.value == "for") {
                i++;
                if (i <= end && tokens[i].value == "(") {
                    int loopStart = i;
                    int loopEnd = findMatchingParen(i);
                    
                    LoopInfo loopInfo = analyzeLoopComplexity(loopStart, loopEnd);
                    
                    i = loopEnd + 1;
                    
                    // Find loop body
                    Complexity bodyComplexity = Complexity::constant();
                    if (i <= end && tokens[i].value == "{") {
                        int bodyEnd = findMatchingBrace(i);
                        bodyComplexity = analyzeBlock(i + 1, bodyEnd - 1, currentFunc);
                        i = bodyEnd + 1;
                    } else {
                        // Single statement body
                        int stmtStart = i;
                        while (i <= end && tokens[i].value != ";") i++;
                        bodyComplexity = analyzeBlock(stmtStart, i - 1, currentFunc);
                        i++;
                    }
                    
                    total = total.max(loopInfo.iterationComplexity * bodyComplexity);
                    continue;
                }
            }
            
            // While loop
            if (t.type == KEYWORD && t.value == "while") {
                i++;
                if (i <= end && tokens[i].value == "(") {
                    int condEnd = findMatchingParen(i);
                    i = condEnd + 1;
                    
                    Complexity bodyComplexity = Complexity::constant();
                    if (i <= end && tokens[i].value == "{") {
                        int bodyEnd = findMatchingBrace(i);
                        bodyComplexity = analyzeBlock(i + 1, bodyEnd - 1, currentFunc);
                        i = bodyEnd + 1;
                    } else {
                        int stmtStart = i;
                        while (i <= end && tokens[i].value != ";") i++;
                        bodyComplexity = analyzeBlock(stmtStart, i - 1, currentFunc);
                        i++;
                    }
                    
                    total = total.max(Complexity::linear() * bodyComplexity);
                    continue;
                }
            }
            
            // Do-while loop
            if (t.type == KEYWORD && t.value == "do") {
                i++;
                
                Complexity bodyComplexity = Complexity::constant();
                if (i <= end && tokens[i].value == "{") {
                    int bodyEnd = findMatchingBrace(i);
                    bodyComplexity = analyzeBlock(i + 1, bodyEnd - 1, currentFunc);
                    i = bodyEnd + 1;
                } else {
                    int stmtStart = i;
                    while (i <= end && tokens[i].value != "while") i++;
                    bodyComplexity = analyzeBlock(stmtStart, i - 1, currentFunc);
                }
                
                // Skip "while" condition
                while (i <= end && tokens[i].value != ";") i++;
                total = total.max(Complexity::linear() * bodyComplexity);
                i++;
                continue;
            }
            
            // If/else
            if (t.type == KEYWORD && (t.value == "if" || t.value == "else")) {
                i++;
                if (i <= end && tokens[i].value == "(") {
                    i = findMatchingParen(i) + 1;
                }
                
                Complexity branchComplexity = Complexity::constant();
                if (i <= end && tokens[i].value == "{") {
                    int bodyEnd = findMatchingBrace(i);
                    branchComplexity = analyzeBlock(i + 1, bodyEnd - 1, currentFunc);
                    i = bodyEnd + 1;
                } else {
                    int stmtStart = i;
                    while (i <= end && tokens[i].value != ";" && tokens[i].value != "else") i++;
                    branchComplexity = analyzeBlock(stmtStart, i - 1, currentFunc);
                }
                total = total.max(branchComplexity);
                continue;
            }
            
            // Nested block
            if (tokens[i].value == "{") {
                int blockEnd = findMatchingBrace(i);
                Complexity blockComplexity = analyzeBlock(i + 1, blockEnd - 1, currentFunc);
                total = total.max(blockComplexity);
                i = blockEnd + 1;
                continue;
            }
            
            // Function calls
            if (t.type == IDENTIFIER && i + 1 <= end && tokens[i + 1].value == "(") {
                string funcName = t.value;
                
                // Check for recursive call
                if (funcName == currentFunc) {
                    if (functions.count(funcName)) {
                        functions[funcName].recursiveCalls++;
                        functions[funcName].isRecursive = true;
                    }
                }
                
                // Check for known standard library complexities
                if (funcName == "sort" || funcName == "stable_sort" || funcName == "partial_sort") {
                    total = total.max(Complexity::nlogn());
                }
                else if (funcName == "reverse" || funcName == "find" || funcName == "count" ||
                         funcName == "search" || funcName == "rotate") {
                    total = total.max(Complexity::linear());
                }
                else if (funcName == "binary_search" || funcName == "lower_bound" || 
                         funcName == "upper_bound" || funcName == "equal_range") {
                    total = total.max(Complexity::logn());
                }
                
                // Skip function arguments
                i = findMatchingParen(i + 1) + 1;
                continue;
            }
            
            i++;
        }
        
        return total;
    }
    
    // ==================== RECURSION ANALYSIS ====================
    
    Complexity analyzeRecursion(const FunctionInfo& func) {
        int calls = func.recursiveCalls;
        
        // Analyze recursion pattern
        bool dividesByTwo = false;
        bool dividesByConstant = false;
        bool subtractsOne = false;
        int recursiveDepth = 0;
        
        // Scan function body for patterns
        for (int i = func.bodyStart; i <= func.bodyEnd; i++) {
            string val = tokens[i].value;
            
            // Check for division by 2 (merge sort pattern)
            if (val == "/" && i + 1 <= func.bodyEnd) {
                if (tokens[i + 1].value == "2") {
                    dividesByTwo = true;
                } else if (tokens[i + 1].type == NUMBER) {
                    dividesByConstant = true;
                }
            }
            
            // Check for subtraction by 1
            if (val == "-" && i + 1 <= func.bodyEnd) {
                if (tokens[i + 1].value == "1") {
                    subtractsOne = true;
                }
            }
            if (val == "--" || val == "-=") {
                subtractsOne = true;
            }
            
            // Check for recursive call pattern
            if (tokens[i].type == IDENTIFIER && tokens[i].value == func.name) {
                if (i + 1 <= func.bodyEnd && tokens[i + 1].value == "(") {
                    recursiveDepth++;
                    
                    // Check arguments for n/2 pattern
                    int parenEnd = findMatchingParen(i + 1);
                    for (int j = i + 2; j < parenEnd; j++) {
                        if (tokens[j].value == "/" && j + 1 < parenEnd) {
                            if (tokens[j + 1].value == "2") {
                                dividesByTwo = true;
                            }
                        }
                    }
                }
            }
        }
        
        // Apply Master Theorem patterns
        if (calls == 0) return Complexity::constant();
        
        // Single recursive call
        if (calls == 1) {
            if (dividesByTwo || dividesByConstant) {
                // T(n) = T(n/2) + O(1) → O(log n)
                return Complexity::logn();
            }
            if (subtractsOne) {
                // T(n) = T(n-1) + O(1) → O(n)
                return Complexity::linear();
            }
            return Complexity::linear();
        }
        
        // Two recursive calls (like merge sort)
        if (calls == 2) {
            if (dividesByTwo) {
                // T(n) = 2T(n/2) + O(n) for merge sort
                // But we need to check if there's O(n) work in merge
                bool hasLinearWork = false;
                
                // Look for merge operation (O(n) work)
                for (int i = func.bodyStart; i <= func.bodyEnd; i++) {
                    string val = tokens[i].value;
                    if (val == "while" || val == "for") {
                        hasLinearWork = true;
                        break;
                    }
                    if (val == "merge" || (i + 3 < func.bodyEnd && 
                        tokens[i].value == "merge" && tokens[i+1].value == "(")) {
                        hasLinearWork = true;
                        break;
                    }
                }
                
                if (hasLinearWork) {
                    return Complexity::nlogn();  // O(n log n)
                } else {
                    return Complexity::linear();  // O(n)
                }
            }
            if (subtractsOne) {
                // T(n) = 2T(n-1) + O(1) → O(2^n)
                return Complexity::exponentialC();
            }
            return Complexity::exponentialC();
        }
        
        // More than 2 recursive calls
        if (calls > 2) {
            if (dividesByTwo) {
                // T(n) = kT(n/2) + O(1) → O(n^log2(k))
                int exponent = ceil(log2(calls));
                return {exponent, 0, 0, 0, false};
            }
            return Complexity::exponentialC();
        }
        
        return Complexity::exponentialC();
    }
    
    // ==================== PARSING ====================
    
    void parseFunctions() {
        pos = 0;
        
        while (pos < tokens.size()) {
            // Look for function definition
            int start = pos;
            
            // Skip keywords like static, const, etc.
            while (pos < tokens.size() && tokens[pos].type == KEYWORD &&
                   (tokens[pos].value == "static" || tokens[pos].value == "const" ||
                    tokens[pos].value == "inline" || tokens[pos].value == "virtual" ||
                    tokens[pos].value == "extern")) {
                pos++;
            }
            
            // Collect return type
            string returnType;
            if (pos < tokens.size() && (tokens[pos].type == KEYWORD || tokens[pos].type == IDENTIFIER)) {
                returnType = tokens[pos].value;
                pos++;
                
                // Handle pointer/reference return types
                while (pos < tokens.size() && (tokens[pos].value == "*" || tokens[pos].value == "&")) {
                    returnType += tokens[pos].value;
                    pos++;
                }
                
                // Check for function name
                if (pos < tokens.size() && tokens[pos].type == IDENTIFIER) {
                    string funcName = tokens[pos].value;
                    pos++;
                    
                    // Check for function parameters
                    if (pos < tokens.size() && tokens[pos].value == "(") {
                        int paramStart = pos;
                        int paramEnd = findMatchingParen(pos);
                        pos = paramEnd + 1;
                        
                        // Check for function body
                        if (pos < tokens.size() && tokens[pos].value == "{") {
                            FunctionInfo func;
                            func.name = funcName;
                            func.returnType = returnType;
                            func.startToken = start;
                            func.bodyStart = pos;
                            
                            int bodyEnd = findMatchingBrace(pos);
                            func.bodyEnd = bodyEnd;
                            func.endToken = bodyEnd;
                            
                            functions[funcName] = func;
                            pos = bodyEnd + 1;
                            continue;
                        }
                    }
                }
            }
            
            pos = start + 1;
        }
    }
    
    void analyzeFunctions() {
        // First pass: detect recursion
        for (auto& [name, func] : functions) {
            for (int i = func.bodyStart; i <= func.bodyEnd; i++) {
                if (tokens[i].type == IDENTIFIER && tokens[i].value == name) {
                    if (i + 1 <= func.bodyEnd && tokens[i + 1].value == "(") {
                        func.isRecursive = true;
                        func.recursiveCalls++;
                    }
                }
            }
        }
        
        // Second pass: analyze complexity
        for (auto& [name, func] : functions) {
            if (func.isRecursive) {
                func.complexity = analyzeRecursion(func);
            } else {
                func.complexity = analyzeBlock(func.bodyStart + 1, func.bodyEnd - 1, name);
            }
        }
    }
    
public:
    ComplexityAnalyzer(const string& code) {
        Tokenizer tokenizer(code);
        tokens = tokenizer.tokenize();
    }
    
    void analyze() {
        parseFunctions();
        analyzeFunctions();
    }
    
    void printResults() {
        cout << "\n==================== ANALYSIS RESULTS ====================\n\n";
        
        if (functions.empty()) {
            cout << "No functions found in the code.\n";
            return;
        }
        
        for (const auto& [name, func] : functions) {
            cout << "Function: " << name << "()\n";
            cout << "  Return Type: " << (func.returnType.empty() ? "void" : func.returnType) << "\n";
            cout << "  Time Complexity: " << func.complexity.toString() << "\n";
            if (func.isRecursive) {
                cout << "  [Recursive with " << func.recursiveCalls << " self-call(s)]\n";
            }
            cout << "\n";
        }
    }
    
    // Space complexity analysis
    void analyzeSpace() {
        cout << "\n==================== SPACE ANALYSIS ====================\n\n";
        
        set<string> arrays, vectors, maps, sets;
        bool dynamicAlloc = false;
        
        for (int i = 0; i < tokens.size(); i++) {
            // Array declarations
            if (tokens[i].type == IDENTIFIER) {
                if (i + 1 < tokens.size() && tokens[i + 1].value == "[") {
                    arrays.insert(tokens[i].value);
                }
            }
            
            // STL containers
            if (tokens[i].value == "vector") vectors.insert("vector");
            if (tokens[i].value == "map" || tokens[i].value == "unordered_map") maps.insert("map");
            if (tokens[i].value == "set" || tokens[i].value == "unordered_set") sets.insert("set");
            
            // Dynamic allocation
            if (tokens[i].value == "new" || tokens[i].value == "malloc" || tokens[i].value == "calloc") {
                dynamicAlloc = true;
            }
        }
        
        cout << "Arrays detected: ";
        if (arrays.empty()) cout << "none";
        else for (const auto& a : arrays) cout << a << " ";
        cout << "\n";
        
        cout << "Vectors: " << (vectors.empty() ? "none" : "detected") << "\n";
        cout << "Maps: " << (maps.empty() ? "none" : "detected") << "\n";
        cout << "Sets: " << (sets.empty() ? "none" : "detected") << "\n";
        cout << "Dynamic Allocation: " << (dynamicAlloc ? "yes" : "no") << "\n";
        
        // Recursive stack space
        for (const auto& [name, func] : functions) {
            if (func.isRecursive) {
                cout << "\nRecursive function '" << name << "' uses O(recursion depth) stack space\n";
            }
        }
        
        cout << "\nNote: Space complexity is typically O(n) for containers/arrays\n";
    }
};

// ==================== FILE READER ====================

string readFile(const string& filename) {
    ifstream file(filename);
    if (!file) {
        cerr << "Error: Cannot open file '" << filename << "'\n";
        exit(1);
    }
    return string((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
}

// ==================== MAIN ====================

int main() {
    cout << "╔══════════════════════════════════════════════════════════╗\n";
    cout << "║         C++ Complexity Analyzer (Improved)               ║\n";
    cout << "╚══════════════════════════════════════════════════════════╝\n";
    
    // Read from input.cpp file
    string code = readFile("input.cpp");
    
    ComplexityAnalyzer analyzer(code);
    analyzer.analyze();
    analyzer.printResults();
    analyzer.analyzeSpace();
    
    cout << "\n══════════════════════════════════════════════════════════\n";
    return 0;
}