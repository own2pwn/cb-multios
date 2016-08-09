#include <string>

#include "clang/AST/AST.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Refactoring.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::driver;
using namespace clang::tooling;

const std::string whitelist[] = { 
  "main", "_terminate", "transmit", "receive", "fdwait", 
  "allocate", "deallocate", "random", "setjmp", "longjmp",
  "sinf", "sin", "sinl",
  "cosf", "cos", "cosl",
  "tanf", "tan", "tanl",
  "logf", "log", "logl",
  "rintf", "rint", "rintl",
  "sqrtf", "sqrt", "sqrtl",
  "fabsf", "fabs", "fabsl",
  "log2f", "log2", "log2l",
  "exp2f", "exp2", "exp2l",
  "expf", "exp", "expl",
  "log10f",  
  "log10",  
  "log10l", 
  "powf", 
  "pow", 
  "powl", 
  "atan2f", 
  "atan2", 
  "atan2l", 
  "remainderf", 
  "remainder", 
  "remainderl", 
  "scalbnf", 
  "scalbn", 
  "scalbnl", 
  "scalblnf", 
  "scalbln", 
  "scalblnl", 
  "significandf", 
  "significand", 
  "significandl"
};


std::set<std::string> names;
std::set<std::string> filenames;

static llvm::cl::OptionCategory ToolingSampleCategory("Matcher Sample");

class SymbolExtractorHandler : public MatchFinder::MatchCallback {
public:
  virtual void run(const MatchFinder::MatchResult &Result) {

	std::string name;
	std::string filename;
    auto &SM = Result.Context->getSourceManager();

    if (const CallExpr *CE = Result.Nodes.getNodeAs<clang::CallExpr>("callexpr")) 
    {

      auto FSS = CE->getLocStart();
      filename = SM.getFilename(FSS);
      filenames.insert(filename);

      const FunctionDecl *FD = CE->getDirectCallee();
	  auto SS = FD->getLocStart();
	  filename = SM.getFilename(SS);

    }
    else if (const FunctionDecl *FD = Result.Nodes.getNodeAs<clang::FunctionDecl>("functionDef"))
	{

	  name = FD->getNameInfo().getAsString();

	  auto SS = FD->getLocStart();
	  filename = SM.getFilename(SS);
	} 
	else if (const TypedefNameDecl *TD_ = Result.Nodes.getNodeAs<clang::TypedefNameDecl>("typedef"))
	{

	  if (TD_->isImplicit()) { return; }

	  auto TD = TD_->getFirstDecl();
	  name = TD->getNameAsString();

	  auto SS = TD->getLocStart();
	  filename = SM.getFilename(SS);
	} 

	for (auto W : whitelist) {
	  if (name == W) {
		return;
	  }
	}

	std::string cgcprefix("cgc_");
	if (!name.compare(0, cgcprefix.size(), cgcprefix))
	  return;

    names.insert(name);
    filenames.insert(filename);

  }

};

int main(int argc, const char **argv) {
  CommonOptionsParser op(argc, argv, ToolingSampleCategory);
  RefactoringTool Tool(op.getCompilations(), op.getSourcePathList());

  SymbolExtractorHandler FH;

  MatchFinder Finder;
  Finder.addMatcher(functionDecl().bind("functionDef"), &FH);
  Finder.addMatcher(typedefDecl().bind("typedef"), &FH);
  Finder.addMatcher(callExpr(callee(functionDecl())).bind("callexpr"), &FH);

  if (int Result = Tool.run(newFrontendActionFactory(&Finder).get())) {
	return Result;
  }

  for (auto N : names)
  {
    if (N.length() == 0)
      continue;

    if (N.find("__builtin") != std::string::npos)
      continue;


    for (auto F : filenames)
    {
      if (F.find("include/libcgc.h") != std::string::npos)
        continue;

      if (F.length() == 0)
        continue;

      char sed_line[1024] = {};

      snprintf(sed_line, 1024, "sed -i 's/\\b%s\\b/cgc_\\0/g' %s", N.c_str(), F.c_str());
      llvm::outs() << sed_line << "\n";

    }
  }

  return 0;
}
