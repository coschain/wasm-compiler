//#include <fc/exception/exception.hpp>
//#include <fc/io/json.hpp>
//#include <contento/rpc_api_generator/rpc_api_generator.hpp>
#include <contento/abi_generator/abi_generator.hpp>
//#include <fc/variant_object.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/format.hpp>

using namespace contento;
using namespace contento::chain;

//using mvo = fc::mutable_variant_object;

std::unique_ptr<FrontendActionFactory> create_factory(bool verbose, bool opt_sfs, string abi_context, abi_def& output, const string& contract, const vector<string>& actions) {

  struct abi_frontend_action_factory : public FrontendActionFactory {

    bool                   verbose;
    bool                   opt_sfs;
    string                 abi_context;
    abi_def&               output;
    const string&          contract;
    const vector<string>&  actions;

    abi_frontend_action_factory(bool verbose, bool opt_sfs, string abi_context,
      abi_def& output, const string& contract, const vector<string>& actions) : verbose(verbose),
      abi_context(abi_context), output(output), contract(contract), actions(actions) {}

    clang::FrontendAction *create() override {
      return new generate_abi_action(verbose, opt_sfs, abi_context, output, contract, actions);
    }

  };

  return std::unique_ptr<FrontendActionFactory>(
      new abi_frontend_action_factory(verbose, opt_sfs, abi_context, output, contract, actions)
  );
}

std::unique_ptr<FrontendActionFactory> create_find_macro_factory(string& contract, vector<string>& actions, string abi_context,abi_def& output) {

  struct abi_frontend_macro_action_factory : public FrontendActionFactory {

    string&          contract;
    vector<string>&  actions;
    string           abi_context;
    abi_def&               output;

    abi_frontend_macro_action_factory (string& contract, vector<string>& actions,
      string abi_context,abi_def& output) : contract(contract), actions(actions), abi_context(abi_context), output(output) {}

    clang::FrontendAction *create() override {
      return new find_contento_abi_macro_action(contract, actions, abi_context,output);
    }

  };

  return std::unique_ptr<FrontendActionFactory>(
    new abi_frontend_macro_action_factory(contract, actions, abi_context,output)
  );
}

static cl::OptionCategory abi_generator_category("ABI generator options");

static cl::opt<std::string> abi_context(
    "context",
    cl::desc("ABI context"),
    cl::cat(abi_generator_category));

static cl::opt<std::string> abi_destination(
    "destination-file",
    cl::desc("destination json file"),
    cl::cat(abi_generator_category));

static cl::opt<bool> abi_verbose(
    "verbose",
    cl::desc("show debug info"),
    cl::cat(abi_generator_category));

static cl::opt<bool> abi_opt_sfs(
    "optimize-sfs",
    cl::desc("Optimize single field struct"),
    cl::cat(abi_generator_category));


void write_json_data_into_string(boost::property_tree::ptree& item)
{
   std::stringstream is;
   boost::property_tree::write_json(is,item);
   std::string s = is.str();
   cout<<"json s:"<<s<<endl;
}

int main(int argc, const char **argv) { abi_def output; try {
   CommonOptionsParser op(argc, argv, abi_generator_category);
   ClangTool Tool(op.getCompilations(), op.getSourcePathList());

   string contract;
   vector<string> actions;
   int result = Tool.run(create_find_macro_factory(contract, actions, abi_context,output).get());
   if(!result) {
      result = Tool.run(create_factory(abi_verbose, abi_opt_sfs, abi_context, output, contract, actions).get());
      if(!result) {
         abi_serializer(output).validate();

          /*
         boost::property_tree::ptree tree;

         output.to_json(tree);
         boost::property_tree::write_json(abi_destination,tree);
           */
          json result;
          output.to_json2(result);
          std::ofstream os(abi_destination);
          os << std::setw(4) << result << std::endl;
      }
   }
   return result;
} FC_CAPTURE_AND_LOG((output)); return -1; }
