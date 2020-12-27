//****************************************************************
//* This file is part of the AsFem framework
//* A Simple Finite Element Method program (AsFem)
//* All rights reserved, Yang Bai @ CopyRight 2020
//* https://github.com/yangbai90/AsFem.git
//* Licensed under GNU GPLv3, please see LICENSE for details
//* https://www.gnu.org/licenses/gpl-3.0.en.html
//****************************************************************
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++ Author : Yang Bai
//+++ Date   : 2020.06.30
//+++ Purpose: Function for reading the whole input file
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#include "InputSystem/InputSystem.h"

bool InputSystem::ReadInputFile(Mesh &mesh,
                                DofHandler &dofHandler,
                                ElmtSystem &elmtSystem,
                                MateSystem &mateSystem,
                                BCSystem &bcSystem,
                                ICSystem &icSystem,
                                FE &fe,
                                SolutionSystem &solutionSystem,
                                OutputSystem &outputSystem,
                                NonlinearSolver &nonlinearSolver){
    ifstream in;
    string str;
    int linenum=0;

    bool HasMeshBlock=false;
    bool HasDofsBlock=false;
    bool HasElmtBlock=false;
    bool HasMateBlock=false;
    bool HasBCBlock=false;
    bool HasICBlock=false;
    bool HasQPointBlock=false;
    bool HasOutputBlock=false;
    bool HasProjectionBlock=false;
    bool HasNonlinearSolverBlock=false;

    if(_HasInputFileName){
        in.open(_InputFileName.c_str(),ios::in);
        while(!in.is_open()){
            MessagePrinter::PrintErrorTxt("can\'t open the input file");
            PetscPrintf(PETSC_COMM_WORLD,"*** Please enter the correct input file name:");
            cin>>_InputFileName;
        }
    }
    else{
        PetscPrintf(PETSC_COMM_WORLD,"*** Please enter the correct input file name:");
            cin>>_InputFileName;
        in.open(_InputFileName.c_str(),ios::in);
        while(!in.is_open()){
            MessagePrinter::PrintErrorTxt("can\'t open the input file");
            PetscPrintf(PETSC_COMM_WORLD,"*** Please enter the input file name:");
            cin>>_InputFileName;
        }
        _HasInputFileName=true;
    }

    linenum=0;

    HasMeshBlock=false;
    HasDofsBlock=false;
    HasElmtBlock=false;
    HasMateBlock=false;
    HasBCBlock=false;
    HasICBlock=false;
    HasQPointBlock=false;
    HasOutputBlock=false;
    HasProjectionBlock=false;
    HasNonlinearSolverBlock=false;

    while(!in.eof()){
        getline(in,str);linenum+=1;
        str=StringUtils::RemoveStrSpace(str);
        str=StringUtils::StrToLower(str);
        if(StringUtils::IsCommentLine(str)||str.size()<1) continue;

        if(str.find("[mesh]")!=string::npos){
            if(!StringUtils::IsBracketMatch(in,linenum)){
                MessagePrinter::PrintErrorTxt("[mesh]/[end] bracket pair dosen\'t match, please check your input file");
                MessagePrinter::AsFem_Exit();
                return false;
            }
            if(ReadMeshBlock(in,str,linenum,mesh)){
                HasMeshBlock=true;
            }
            else{
                HasMeshBlock=false;
            }
        }
        else if(str.find("[dofs]")!=string::npos){
            if(!StringUtils::IsBracketMatch(in,linenum)){
                MessagePrinter::PrintErrorTxt("[dofs]/[end] bracket pair dosen\'t match, please check your input file");
                MessagePrinter::AsFem_Exit();
                return false;
            }
            if(ReadDofsBlock(in,str,linenum,dofHandler)){
                HasDofsBlock=true;
            }
            else{
                HasDofsBlock=false;
            }
        }
        else if(str.find("[elmts]")!=string::npos&&str.find("[ielmts]")==string::npos){
            if(!HasDofsBlock){
                MessagePrinter::PrintErrorTxt("[elmts] block requires the [dofs] block, you should define the [dofs] block first, then given the [elmts] block");
                return false;
            }
            int lastendlinenum;
            if(StringUtils::IsBracketMatch(in,linenum,lastendlinenum)){
                if(ReadElmtBlock(in,str,lastendlinenum,linenum,elmtSystem,dofHandler)){
                    HasElmtBlock=true;
                }
                else{
                    HasElmtBlock=false;
                }
            }
            else{
                MessagePrinter::PrintErrorTxt("[elmts]/[end] bracket pair is not match, please check your input file");
                MessagePrinter::AsFem_Exit();
                return false;
            }
        }
        else if(str.find("[mates]")!=string::npos){
            int lastendlinenum;
            if(StringUtils::IsBracketMatch(in,linenum,lastendlinenum)){
                if(ReadMateBlock(in,str,lastendlinenum,linenum,mateSystem)){
                    HasMateBlock=true;
                }
                else{
                    HasMateBlock=false;
                }
            }
            else{
                MessagePrinter::PrintErrorTxt("[mates]/[end] bracket pair is not match, please check your input file");
                MessagePrinter::AsFem_Exit();
                return false;
            }
        }
        else if(str.find("[bcs]")!=string::npos){
            if(!HasDofsBlock){
                MessagePrinter::PrintErrorTxt("[bcs] block requires the [dofs] block, you should define the [dofs] block before the [bcs] block");
                MessagePrinter::AsFem_Exit();
                return false;
            }
            int lastendlinenum;
            if(StringUtils::IsBracketMatch(in,linenum,lastendlinenum)){
                if(ReadBCBlock(in,str,lastendlinenum,linenum,bcSystem,dofHandler)){
                    HasBCBlock=true;
                }
                else{
                    HasBCBlock=false;
                }
            }
            else{
                MessagePrinter::PrintErrorTxt("[bcs]/[end] bracket pair is not match, please check your input file");
                MessagePrinter::AsFem_Exit();
                return false;
            }
        }
        else if(str.find("[ics]")!=string::npos){
            if(!HasDofsBlock){
                MessagePrinter::PrintErrorTxt("[ics] block requires the [dofs] block, you should define the [dofs] block before the [ics] block");
                MessagePrinter::AsFem_Exit();
                return false;
            }
            int lastendlinenum;
            if(StringUtils::IsBracketMatch(in,linenum,lastendlinenum)){
                if(ReadICBlock(in,str,lastendlinenum,linenum,icSystem,dofHandler)){
                    HasICBlock=true;
                }
                else{
                    HasICBlock=false;
                    MessagePrinter::AsFem_Exit();
                }
            }
            else{
                MessagePrinter::PrintErrorTxt("[ics]/[end] bracket pair is not match, please check your input file");
                MessagePrinter::AsFem_Exit();
                return false;
            }
        }
        else if(str.find("[qpoint]")!=string::npos){
            if(!HasMeshBlock){
                MessagePrinter::PrintErrorTxt("[qpoint] block requires the [mesh] block, you should define the [mesh] block first, then given the [qpoint] block");
                return false;
            }
            int lastendlinenum;
            if(StringUtils::IsBracketMatch(in,linenum,lastendlinenum)){
                if(ReadQPointBlock(in,str,linenum,fe)){
                    HasQPointBlock=true;
                }
                else{
                    HasQPointBlock=false;
                }
            }
            else{
                MessagePrinter::PrintErrorTxt("[qpoint]/[end] bracket pair is not match, please check your input file");
                MessagePrinter::AsFem_Exit();
                return false;
            }
        }
        else if((str.find("[output]")!=string::npos)&&str.length()==8){
            int lastendlinenum;
            if(StringUtils::IsBracketMatch(in,linenum,lastendlinenum)){
                if(ReadOutputBlock(in,str,linenum,outputSystem)){
                    HasOutputBlock=true;
                }
                else{
                    HasOutputBlock=false;
                }
            }
            else{
                MessagePrinter::PrintErrorTxt("[output]/[end] bracket pair is not match, please check your input file");
                MessagePrinter::AsFem_Exit();
                return false;
            }
        }
        else if((str.find("[projection]")!=string::npos)&&str.length()==12){
            int lastendlinenum;
            if(StringUtils::IsBracketMatch(in,linenum,lastendlinenum)){
                if(ReadProjectionBlock(in,str,linenum,solutionSystem)){
                    HasProjectionBlock=true;
                }
                else{
                    HasProjectionBlock=false;
                }
            }
            else{
                MessagePrinter::PrintErrorTxt("[projection]/[end] bracket pair is not match, please check your input file");
                MessagePrinter::AsFem_Exit();
                return false;
            }
        }
        else if((str.find("[nonlinearsolver]")!=string::npos)&&str.length()==17){
            int lastendlinenum;
            if(StringUtils::IsBracketMatch(in,linenum,lastendlinenum)){
                if(ReadNonlinearSolverBlock(in,str,linenum,nonlinearSolver)){
                    HasNonlinearSolverBlock=true;
                }
                else{
                    HasNonlinearSolverBlock=false;
                }
            }
            else{
                MessagePrinter::PrintErrorTxt("[nonlinearsolver]/[end] bracket pair is not match, please check your input file");
                MessagePrinter::AsFem_Exit();
                return false;
            }
        }
        else if(str.find("[]")!=string::npos){
            MessagePrinter::PrintErrorInLineNumber(linenum);
            MessagePrinter::PrintErrorTxt("the bracket pair is not complete in your input file, you should check it",false);
            MessagePrinter::AsFem_Exit();
        }
    }

    if(!HasMeshBlock){
        MessagePrinter::PrintErrorTxt("no [mesh] block is found, for FEM analysis, mesh is required");
        MessagePrinter::AsFem_Exit();
    }

    if(!HasDofsBlock){
        MessagePrinter::PrintErrorTxt("no [dofs] block is found, for FEM analysis, dofs are required");
        MessagePrinter::AsFem_Exit();
    }

    if(!HasElmtBlock){
        if(!_IsReadOnly){
            MessagePrinter::PrintErrorTxt("no [elmts] block is found, for FEM analysis, elmts are required");
            MessagePrinter::AsFem_Exit();
        }
    }

    if(!HasMateBlock){
        if(!_IsReadOnly){
            MessagePrinter::PrintWarningTxt("no [mates] block is found, the default material values will be used by the [elmts]",false);
        }
    }

    if(!HasBCBlock){
        if(!_IsReadOnly){
            MessagePrinter::PrintWarningTxt("no [bcs] block is found, no any boundary conditions will be used by AsFem",false);
        }
    }

    if(!HasICBlock){
        if(!_IsReadOnly){
            MessagePrinter::PrintWarningTxt("no [ics] block is found, no any initial conditions will be used by AsFem",false);
        }
    }

    if(!HasQPointBlock){
        if(!_IsReadOnly){
            MessagePrinter::PrintWarningTxt("no [qpoint] block is found, default gauss-legendre integration options will be used by AsFem",false);
        }
        fe.SetDim(mesh.GetDim());
        fe.SetQPointType(QPointType::GAUSSLEGENDRE);
        fe.SetBulkQpOrder(mesh.GetBulkMeshOrder()+1);
        fe.SetBCQpOrder(mesh.GetBulkMeshOrder()+1);
        fe.CreateQPoints(mesh);
    }
    else{
        fe.SetDim(mesh.GetDim());
        fe.CreateQPoints(mesh);
    }


    if(!HasProjectionBlock){
        if(!_IsReadOnly){
            MessagePrinter::PrintWarningTxt("no [projection] block is found, the default projection name will be used by AsFem",false);
        }
        solutionSystem.PrintProjectionInfo();
    }
    else{
        solutionSystem.PrintProjectionInfo();
    }

    if(!HasOutputBlock){
        if(!_IsReadOnly){
            MessagePrinter::PrintWarningTxt("no [output] block is found, default output options will be used by AsFem",false);
        }
        outputSystem.Init(_InputFileName);
    }

    if(!HasNonlinearSolverBlock){
        if(!_IsReadOnly){
            MessagePrinter::PrintWarningTxt("no [nonlinearsolver] block is found, default newton-raphson with line search solver options will be used by AsFem",false);
        }
    }

    MessagePrinter::PrintDashLine();

    return true;
}