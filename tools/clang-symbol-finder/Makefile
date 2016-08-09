
CXXFLAGS := $(shell llvm-config --cxxflags)
LDFLAGS := $(shell llvm-config --ldflags)

CLANG_LIBS := \
	-lclangAST \
	-lclangASTMatchers \
	-lclangAnalysis \
	-lclangBasic \
	-lclangDriver \
	-lclangEdit \
	-lclangFrontend \
	-lclangFrontendTool \
	-lclangLex \
	-lclangParse \
	-lclangSema \
	-lclangEdit \
	-lclangRewrite \
	-lclangRewriteFrontend \
	-lclangStaticAnalyzerFrontend \
	-lclangStaticAnalyzerCheckers \
	-lclangStaticAnalyzerCore \
	-lclangSerialization \
	-lclangToolingCore \
	-lclangTooling \
	-lclangFormat \
	-lclang

LLVM_LIBS := $(shell llvm-config --libs) $(shell llvm-config --system-libs)

ALL:
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(CLANG_LIBS) $(LLVM_LIBS) -o csf csf.cpp 
