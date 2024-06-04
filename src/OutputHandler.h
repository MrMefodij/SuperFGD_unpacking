#ifndef _OUTPUTHANDLER_H_
#define _OUTPUTHANDLER_H_


#include <string>
#include <filesystem>


class OutputHandler{
    public:
    OutputHandler();
    std::string GetHostName();
    std::string GetUserName();
    std::string GetOutputFilePrefix(std::string filename, std::string outdirname);
    void SetVerbose(int v){fVerbose=v;};

    private:
    std::filesystem::path fPath;
    std::string fFileName; // from argv (relative path)
    std::string fOutputDirName; // from argv (if any)
    std::string fFileNamePrefix;
    std::string fAbsolutePath;
    std::string fParentPath; 
    std::string fOutputFilePrefix;

    std::string fHostName;
    std::string fUserName;

    bool fOutputDir;
    bool fKEKCC;
    int fVerbose;

    const std::string fKEKCCDATAPATH="/gpfs/group/t2k/nd280/SuperFGDCommissioning/data";

    static const int fNumKeyword=2;
    const std::string fKeyword[fNumKeyword]={"Calibration/","Cosmics/"};
    
};
#endif
