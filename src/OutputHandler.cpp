#include <iostream>
#include <iomanip>
#include <unistd.h>

#include "OutputHandler.h"

/////
OutputHandler::OutputHandler()
/////
{
    // --- Init
    fKEKCC=fOutputDir=0;
    fVerbose=0;
    fHostName=fUserName="";

    // --- Get Hostname & UserName
    GetHostName();
    GetUserName();
    if(fHostName.find("cc.kek.jp")!=std::string::npos) fKEKCC = 1;
}


/////
std::string OutputHandler::GetOutputFilePrefix(std::string filename, std::string outdirname)
/////
{
    // --- Init
    fFileName = filename;
    fOutputDirName = outdirname;
    fFileNamePrefix=fAbsolutePath=fParentPath=fOutputFilePrefix="";

    //  --- Is output directory defined ?
    if(fOutputDirName.length()>0) fOutputDir=1;

    // --- Get absoulte path, parent path, filename prefix 
    fPath = fFileName;
    fAbsolutePath = std::filesystem::absolute(fPath);
    fParentPath = std::filesystem::absolute(fPath).parent_path();
    fFileNamePrefix = fPath.stem();

    if(!fKEKCC){ // Not in KEKCC
        if(!fOutputDir){// output directory is not defined -> Outputs in the same dir.
            fOutputFilePrefix = fParentPath + "/" + fFileNamePrefix;
        }else{  // output directory is specifiyed
            //std::filesystem::create_directories(fOutputDirName); // mkdir
            fOutputFilePrefix = fOutputDirName + "/" + fFileNamePrefix;
        }
    }else{ // In KEKCC
        std::cout << "!!! KEKCC !!!" << std::endl;
        if(fOutputDir){ // output directory is determined
            fOutputFilePrefix = fOutputDirName + "/" + fFileNamePrefix;
        }else if(fAbsolutePath.find(fKEKCCDATAPATH.c_str())==std::string::npos){
            // Not common directory -> outputs in the same dir
            fOutputFilePrefix = fParentPath + "/" + fFileNamePrefix;
        }else{// Common directory && output directory is not determined
            std::cout << "You did not define output direcotry." << std::endl;
            std::cout << "The output directory is automatically determined and created" << std::endl;
            bool findkeyword=0;
            int position=-1;
            for(int i=0;i<fNumKeyword;i++){
                if(fParentPath.find(fKeyword[i].c_str())!=std::string::npos){
                    position = fParentPath.find(fKeyword[i].c_str()) + fKeyword[i].length();
                    findkeyword=1;
                    break;
                }
            }
            if(!findkeyword){
                std::cerr << "!!! This directory is not suppoorted" << std::endl;
                std::cerr << "!!! Please contact S.Abe (seisho@icrr.u-tokyo.ac.jp)" << std::endl;
                exit(1);
            }
            std::string AutoOutputDir = fParentPath.substr(0,position) + "unpacked_" + fUserName
                        + "/" + fParentPath.substr(position);
            std::filesystem::create_directories(AutoOutputDir);
            fOutputFilePrefix = AutoOutputDir + "/" + fFileNamePrefix;
        }
    }
    
    if(fVerbose>=1){
        std::cout << std::setw(20) << "fHostName: " << fHostName << std::endl;
        std::cout << std::setw(20) << "fUserName: " << fUserName << std::endl;
        std::cout << std::setw(20) << "fPath.string(): " << fPath.string() << std::endl;
        std::cout << std::setw(20) << "fAbsolutePath: " << fAbsolutePath << std::endl;
        std::cout << std::setw(20) << "fParentPath: " << fParentPath << std::endl;
        std::cout << std::setw(20) << "fFileNamePrefix: " << fFileNamePrefix << std::endl;
        std::cout << std::setw(20) << "fOutputFilePrefix: " << fOutputFilePrefix << std::endl;
    }
    return fOutputFilePrefix;
}

/////
std::string OutputHandler::GetHostName()
/////
{
    if(fHostName.length()==0){
        const int HOSTNAME_MAX=50;
        char hostname_c[HOSTNAME_MAX];
        gethostname(hostname_c,HOSTNAME_MAX);
        fHostName = hostname_c;
    }
    return fHostName;
}


/////
std::string OutputHandler::GetUserName()
/////
{
    if(fUserName.length()==0){
        if(getenv("USER")!=NULL){
            fUserName = getenv("USER");
        }else if(getenv("USERNAME")!=NULL){
            fUserName = getenv("USERNAME");
        }else{
            std::cerr << "Failed to get username" << std::endl;
        }
    }
    return fUserName;
}


