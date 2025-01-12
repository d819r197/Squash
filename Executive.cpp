#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <bits/stdc++.h>

#include "Executive.h"
#include "Quash.h"

//Executive Class Methods
Executive::Executive(char **envp) {
  std::string unparsedPath = "";
  std::string* path;
  std::string home;
  std::string pwd;

  for(int lcv=0;envp[lcv] != nullptr; lcv++) {
    // std::cout << envp[lcv] <<std::endl;
    std::string envVar = envp[lcv];
      if(envVar.substr(0,5) == "PATH=") {
        unparsedPath = envVar.substr(5, envVar.size());
      }
      else if(envVar.substr(0,5) == "HOME=") {
        home = envVar.substr(5, envVar.size());
      }
      else if(envVar.substr(0,4) == "PWD=") {
        pwd = envVar.substr(4, envVar.size());
      }
  }

  std::istringstream ps(unparsedPath);
  std::string p;
  int countP=1;
  for(int lcv=0; lcv < unparsedPath.size(); lcv++ ) {
    if(unparsedPath[lcv] == ':') {
      countP++;
    }
  }
  path = new std::string[countP];
  int ind = 0;

  while(std::getline(ps, p, ':')) {
      path[ind] = p;
      ind ++;
  }
  prog = new Quash(path, home, pwd);

  prog->SetPathRaw(unparsedPath);
}


void Executive::Run() {
  bool b = true;
  std::string inp = "";
  while(b){
    bool isBG = false;
    bool inPipe = false;

    bool isReadIn = false;
    int readInPos = 0;
    std::string readCmd;

    bool isWriteOut = false;
    int writeOutPos = 0;
    std::string* writeCmd;

    int cPipe = 0;
    //std::list<int> lPipe;
    int lPipe =0;
    std::cout<<prog->GetPwd() <<"> ";
    //std::getline(std::cin, inp);
    inp = "";
    std::getline(std::cin, inp);
    int countSP=1;
      for(int lcv = 0; lcv < inp.length(); lcv++) {
          //inp[lcv] = std::tolower(inp[lcv]);
          if(inp.size() > 3)
        if(inp[lcv] == ' ' || inp[lcv] == '\0') {
          countSP++;
        }
      }
      //std::cout<<countSP<<std::endl;
      //std::cout<<inp.length()<<std::endl;
    std::istringstream inpA;
    inpA.str(inp);
    std::string inpArgs[countSP+1];
    int ind = 0;

    for(int i =0; i<countSP; i++){
        inpA >> inpArgs[i];
      if(inpArgs[i] == "|"){
        inPipe = true;
        cPipe ++;
        lPipe = i;
      //  lPipe.push_back(i);
      }
      //Read in
      else if(inpArgs[i] == "<"){
        isReadIn = true;
        readInPos = i;
        std::string cmd1[i];
        for(int lcv = 0; lcv < i; lcv++) {
          cmd1[lcv] = inpArgs[lcv];
        }
        readCmd = inpArgs[0];
      }
      else if(inpArgs[i] == ">") {
        isWriteOut = true;
        writeOutPos = i;
        std::string cmd2[i];
        for(int lcv = 0; lcv < i; lcv++) {
          cmd2[lcv] = inpArgs[lcv];
        }
        writeCmd = cmd2;
      }

      //std::cout<<inpArgs[i];
    }

      if(inp[inp.size()-1] == '&')
        isBG = true;
      //user defined path
          //std::cout<<" Inp Args: "<<inpArgs[0];
          // for(int i =0; i<countSP; i++)
          //   std::cout<<" Args: "<<inpArgs[i]


    //non user defined path

    //std::cout<<std::endl;
      if(inp == "quit" || inp == "exit") {
        std::cout <<"Goodbye!\n";
        exit(0);
      }
      else if(inPipe && cPipe > 0){
        std::string leftP[lPipe] = {};
        std::string rightP[countSP-lPipe-1] = {};
        //std::cout<<lPipe<<std::endl;
        //std::cout<<countSP - lPipe-1<<std::endl;
        for(int i =0; i<lPipe; i++){
          leftP[i] = inpArgs[i];
          //std::cout<<leftP[i]<<" ";
        }
        //std::cout<<std::endl;
        for(int i = 0; i<countSP-lPipe-1; i++){
          rightP[i] = inpArgs[i+lPipe+1];
          //std::cout<<rightP[i]<<" ";
        }
        //std::cout<<std::endl;
        prog->Pipe(leftP, rightP);
        inPipe = false;
      }
      else if(inpArgs[0] == "cd") {
        if(countSP == 1) {
          prog->ChangeDir(NULL);
        }
        else if(countSP == 2) {
          if(inpArgs[1][0] == '~') {
            inpArgs[1] = std::string(getenv("HOME")) + inpArgs[1].substr(1,inpArgs[1].size());
            prog->ChangeDir(inpArgs[1].c_str());
          }
          else {
            prog->ChangeDir(inpArgs[1].c_str());
          }
        }
        else {
          std::cout << "Error: cd has extra arguments\n";
        }
      }
      else if(inpArgs[0] == "set") {
        std::cout <<"In Set, arg: " <<inpArgs[1].substr(0,5) <<std::endl;
        if(inpArgs[1].substr(0,5) == "PATH=") {
          std::string* path;
          std::string unparsedPathIn = prog->GetPathRaw() + ':' + inpArgs[1].substr(6,inpArgs[1].size()-1);
          prog->SetPathRaw(unparsedPathIn);
          std::istringstream pathStream(unparsedPathIn);
          std::string p;
          int count=1;
          for(int lcv=0; lcv < unparsedPathIn.size(); lcv++ ) {
            if(unparsedPathIn[lcv] == ':') {
              count++;
            }
          }
          path = new std::string[count];
          int ind = 0;

          while(std::getline(pathStream, p, ':')) {
              path[ind] = p;
              ind ++;
          }
          prog->SetPath(path);
          std::cout <<"Path set to: " <<prog->GetPathRaw() <<std::endl;
        }
        else if(inpArgs[1].substr(0,5) == "HOME=") {
          prog->SetHome(inpArgs[1].substr(5,inpArgs[1].size()));
          std::cout <<"HOME set to: " <<prog->GetHome() <<std::endl;
        }
        else {
          std::cout <<"Error: Set variable was not HOME or PATH. \n";
        }
      }
      else if(inpArgs[0] == "PATH") {
        std::cout << prog->GetPathRaw() <<std::endl;
      }
      else if(inpArgs[0] == "HOME") {
        std::cout << prog->GetHome() <<std::endl;
      }
      else if(inpArgs[0] == "quash" && inpArgs[1] =="<") {
        if(countSP >= 3) {
          prog->Import(inpArgs[2]);
        }
        else {
          std::cout <<"Error: Can not import. No file was given.\n";
        }
      }
      else if(inpArgs[0] == "jobs") {
        prog->ListJobs();
      }
      else if(inpArgs[0] == "kill") {
        if(countSP == 2) {
          size_t found = inpArgs[1].find('%');
          if (found != std::string::npos)  {
            std::string jobNum = inpArgs[1].substr(1, inpArgs[1].size());
            std::cout <<"Killing job number: " <<jobNum <<std::endl;
            std::list<Process*>::iterator p = (prog->GetBgProccessList())->begin();
            std::advance(p, std::stoi(jobNum)-1);
            if((prog->GetBgProccessList())->size() > std::stoi(jobNum)) {
              prog->KillJob(jobNum);
            }
            else {
              std::cout <<"Error: Job " <<jobNum <<" does not exist.\n";
            }
            // p->status = "terminated";
          }
          //Kill with PID
          else {
            prog->KillJob(inpArgs[1]);
            //pop
          }
        }
        else {
          "Error: wrong number of arguments to kill a job\n";
        }
      }
      else if(inpArgs[0][0] == '%') {
        std::string jId = inpArgs[0].substr(1, inpArgs[0].size());
        std::cout <<"Running process with jobID: " <<jId <<std::endl;
        if((prog->GetBgProccessList())->size() > std::stoi(jId)-1 && std::stoi(jId) != 0) {
          std::list<Process*>::iterator p = (prog->GetBgProccessList())->begin();
          std::advance(p, std::stoi(jId)-1);
          Process* currProcess = *p;

          std::string currCmd[currProcess->numOfArgs];
          std::istringstream iss;
          iss.str(currProcess->cmdRaw);
          for (int i =0; i<currProcess->numOfArgs; i++){
            iss >> currCmd[i];
          }

          prog->Run(currCmd, false, currProcess->numOfArgs);
          std::cout <<'[' <<jId <<"] " <<currProcess->pid <<" finished " <<currProcess->cmdRaw <<std::endl;

          (prog->GetBgProccessList())->erase(p);
        }
        else {
          std::cout << "Error: No Background job with job id: " <<jId <<std::endl;
        }
      }
      else if(inpArgs[0] == "fg") {

        if((prog->GetBgProccessList())->empty() == false) {
          Process * newestProcess = (prog->GetBgProccessList())->back();
          (prog->GetBgProccessList())->pop_back();
          //std::cout << "Running process: " <<newestProcess.cmdRaw <<std::endl;
          //std::cout <<"fg) Num of args: " <<newestProcess.numOfArgs <<" Last arg: " <<newestProcess.cmd[newestProcess.numOfArgs-1] <<std::endl;
          std::string currCmd[newestProcess->numOfArgs];
          std::istringstream iss;
          iss.str(newestProcess->cmdRaw);
          for (int i =0; i<newestProcess->numOfArgs; i++){
            iss >> currCmd[i];
          }

          prog->Run(currCmd, false, newestProcess->numOfArgs);
          std::cout <<'[' <<(prog->GetBgProccessList())->size() <<"] " <<newestProcess->pid <<" finished " <<newestProcess->cmdRaw <<std::endl;

        }
        else {
          std::cout <<"Error: There are no jobs currently in the background.\n";
        }
      }
      else if(inpArgs[0] == "bg") {
        if((prog->GetBgProccessList())->empty() == false) {
          Process * newestProcess = (prog->GetBgProccessList())->back();
          // std::cout << "bg) Running process: " <<newestProcess.cmdRaw <<std::endl;

          std::cout <<'[' <<(prog->GetBgProccessList())->size() <<"]+ ";

          std::cout << newestProcess->cmdRaw <<'&' <<std::endl;
        }
        else {
          std::cout <<"Error: There are no jobs currently in the background.\n";
        }
      }
      else if(isWriteOut) {
        if(countSP == writeOutPos+2) {
          std::string pathOut = inpArgs[writeOutPos+1];
          prog->WriteOut(writeCmd, pathOut);
        }
        else {
          std::cout <<"Error: Writing out to a file should only have one argument after the '>'\n";
        }

        // isWriteOut = false;
        // writeOutPos = 0;
        // writeCmd = nullptr;
      }
      else if(isReadIn) {
        if(countSP == readInPos+2) {
          std::string pathIn = inpArgs[readInPos+1];
          prog->ReadIn(readCmd, pathIn);
        }
        else {
          std::cout <<"Error: Reading in from a file should only have one argument after the '<'\n";
        }
        // isReadIn = false;
        // readInPos = 0;
        // readCmd = nullptr;

      }
      else
        prog->Run(inpArgs, isBG, countSP);
    }
}
