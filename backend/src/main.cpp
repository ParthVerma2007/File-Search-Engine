#include "core/SearchEngine.h"
#include "server/HttpServer.h"
#include <iostream>
#include <sstream>

static void printHelp(){
    std::cout<<"\nCommands:\n"
             <<"  search <query>       Search documents\n"
             <<"  autocomplete <word>  Show Trie suggestions\n"
             <<"  stats                Show index statistics\n"
             <<"  index                Show sample indexed terms\n"
             <<"  rebuild              Rebuild the index\n"
             <<"  help                 Show this help\n"
             <<"  exit                 Quit\n\n";
}

int main(int argc,char**argv){
    try{
        std::filesystem::path docs=std::filesystem::path(argv[0]).parent_path().parent_path()/"documents";
        // When launched from backend/build, parent_path().parent_path() is backend.
        SearchEngine engine(docs);

        if(argc>1 && std::string(argv[1])=="--server"){
            HttpServer server(engine,8080);
            server.run();
            return 0;
        }

        std::cout<<"Mini Search Engine\n";
        std::cout<<"Documents: "<<engine.documents().size()<<"\n";
        printHelp();

        std::string line;
        while(std::cout<<"> " && std::getline(std::cin,line)){
            if(line=="exit"||line=="quit")break;
            if(line=="help"){printHelp();continue;}
            if(line=="stats"){
                const auto&s=engine.indexStats();
                std::cout<<"Documents: "<<s.documents<<"\n"
                         <<"Unique terms: "<<s.uniqueTerms<<"\n"
                         <<"Indexed tokens: "<<s.indexedTokens<<"\n";
                continue;
            }
            if(line=="rebuild"){engine.rebuild();std::cout<<"Index rebuilt.\n";continue;}
            if(line=="index"){
                for(const auto&w:engine.index().sample())std::cout<<w<<"\n";
                continue;
            }
            if(line.rfind("autocomplete ",0)==0){
                auto words=engine.autocomplete(line.substr(13));
                for(const auto&w:words)std::cout<<w<<"\n";
                continue;
            }
            if(line.rfind("search ",0)==0){
                SearchStats s;
                auto results=engine.search(line.substr(7),"Binary Search","Quick Sort",s);
                for(const auto&r:results)
                    std::cout<<r.filename<<" | "<<r.score<<" | "<<r.snippet<<"\n";
                std::cout<<"Comparisons: "<<s.comparisons<<" | "<<s.milliseconds<<" ms\n";
                continue;
            }
            std::cout<<"Unknown command. Type help.\n";
        }
    }catch(const std::exception& e){
        std::cerr<<"Error: "<<e.what()<<"\n";
        return 1;
    }
    return 0;
}
