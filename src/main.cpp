#include <iostream>
#include <string>
#include <cstdlib>

enum class Phase {
    SCAN,
    PARSE,
    AST,
    TAC,
    RTL,
    ASM
};

struct Options {
    Phase stopAfter = Phase::ASM;   // default: full compilation
    bool dumpTokens = false;
    bool dumpAST = false;
    bool dumpTAC = false;
    bool dumpRTL = false;
    bool dumpASM = true;            // default behavior in SCLP
    std::string inputFile;
};

void printHelp() {
    std::cout << "Usage: sclp [options] <input>\n"
              << "Options:\n"
              << "  --scan            Stop after scanning\n"
              << "  --parse           Stop after parsing\n"
              << "  --ast             Stop after AST generation\n"
              << "  --tac             Stop after TAC generation\n"
              << "  --rtl             Stop after RTL generation\n"
              << "\n"
              << "Dump Options:\n"
              << "  --tokens          Print tokens\n"
              << "  --print-ast       Print AST\n"
              << "  --print-tac       Print TAC\n"
              << "  --print-rtl       Print RTL\n"
              << "  --print-asm       Print Assembly\n";
}

Options parseArgs(int argc, char* argv[]) {
    Options opts;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--scan") {
            opts.stopAfter = Phase::SCAN;
        } else if (arg == "--parse") {
            opts.stopAfter = Phase::PARSE;
        } else if (arg == "--ast") {
            opts.stopAfter = Phase::AST;
        } else if (arg == "--tac") {
            opts.stopAfter = Phase::TAC;
        } else if (arg == "--rtl") {
            opts.stopAfter = Phase::RTL;
        }

        // dump flags
        else if (arg == "--tokens") {
            opts.dumpTokens = true;
        } else if (arg == "--print-ast") {
            opts.dumpAST = true;
        } else if (arg == "--print-tac") {
            opts.dumpTAC = true;
        } else if (arg == "--print-rtl") {
            opts.dumpRTL = true;
        } else if (arg == "--print-asm") {
            opts.dumpASM = true;
        }

        else if (arg[0] == '-') {
            std::cerr << "Unknown option: " << arg << "\n";
            exit(1);
        } else {
            opts.inputFile = arg;
        }
    }

    return opts;
}

void runCompiler(const Options& opts) {
    std::cout << "[INFO] Input: " << opts.inputFile << "\n";

    // --- SCAN ---
    std::cout << "[PHASE] Scanning...\n";
    if (opts.dumpTokens) {
        std::cout << "[DUMP] Tokens...\n";
    }
    if (opts.stopAfter == Phase::SCAN) return;

    // --- PARSE ---
    std::cout << "[PHASE] Parsing...\n";
    if (opts.stopAfter == Phase::PARSE) return;

    // --- AST ---
    std::cout << "[PHASE] AST Generation...\n";
    if (opts.dumpAST) {
        std::cout << "[DUMP] AST...\n";
    }
    if (opts.stopAfter == Phase::AST) return;

    // --- TAC ---
    std::cout << "[PHASE] TAC Generation...\n";
    if (opts.dumpTAC) {
        std::cout << "[DUMP] TAC...\n";
    }
    if (opts.stopAfter == Phase::TAC) return;

    // --- RTL ---
    std::cout << "[PHASE] RTL Generation...\n";
    if (opts.dumpRTL) {
        std::cout << "[DUMP] RTL...\n";
    }
    if (opts.stopAfter == Phase::RTL) return;

    // --- ASM ---
    std::cout << "[PHASE] Assembly Generation...\n";
    if (opts.dumpASM) {
        std::cout << "[DUMP] Assembly...\n";
    }
}

int main(int argc, char* argv[]) {
    Options opts = parseArgs(argc, argv);

    if (opts.inputFile.empty()) {
        printHelp();
        return 1;
    }

    runCompiler(opts);
    return 0;
}