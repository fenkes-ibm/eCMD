//IBM_PROLOG_BEGIN_TAG
/* 
 * Copyright 2018 IBM International Business Machines Corp.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * 	http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
//IBM_PROLOG_END_TAG


//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#include <plat_mmio_access.H> 

#include <stdio.h>

static const char ECMD_DLL_NOT_LOADED_ERROR[] = ": eCMD Function called before DLL has been loaded\n";
static const char ECMD_UNABLE_TO_FIND_FUNCTION_ERROR[] = ": Unable to find function, must be an invalid DLL - program aborting\n";

#ifndef ECMD_STATIC_FUNCTIONS
#include <fapi2ClientEnums.H>
#include <dlfcn.h>
extern void * dlHandle;
extern void * fapi2DllFnTable[];
#else
#include <fapi2DllCapi.H>
#endif

extern bool fapi2Initialized;
#ifndef ECMD_STRIP_DEBUG
extern int ecmdClientDebug;
extern int fppCallCount;
extern bool ecmdDebugOutput;
#endif

namespace fapi2plat
{

fapi2::ReturnCode getMMIO(const ecmdChipTarget& i_target,
                          const uint64_t i_mmioAddr,
                          const size_t i_transSize,
                          std::vector<uint8_t>& o_data)
{
    const char * dllFunctionName = "dllFapi2GetMMIO";
    fapi2::ReturnCode rc(fapi2::FAPI2_RC_SUCCESS);
    uint32_t l_ecmdRc;

#ifndef ECMD_STATIC_FUNCTIONS
    if (dlHandle == NULL)
    {
        fprintf(stderr,"%s%s",dllFunctionName,ECMD_DLL_NOT_LOADED_ERROR);
        exit(ECMD_DLL_INVALID);
    }
#endif

    if (!fapi2Initialized)
    {
        fprintf(stderr,"%s: eCMD Extension not initialized before function called\n", dllFunctionName);
        fprintf(stderr,"%s: OR eCMD fapi Extension not supported by plugin\n", dllFunctionName);
        exit(ECMD_DLL_INVALID);
    }

#ifndef ECMD_STRIP_DEBUG
    int myTcount;
    std::vector< void * > args;
    if (ecmdClientDebug != 0)
    {
        args.push_back((void*) &i_target);
        args.push_back((void*) &i_mmioAddr);
        args.push_back((void*) &i_transSize);
        args.push_back((void*) &o_data);
        fppCallCount++;
        myTcount = fppCallCount;
        ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"uint32_t getMMIO(const ecmdChipTarget & i_target, const uint64_t i_mmioAddr, const size_t i_transSize, std::vector<uint8_t>& o_data)",args);
        ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"getMMIO");
    }
#endif

#ifdef ECMD_STATIC_FUNCTIONS
    l_ecmdRc = dllFapi2GetMMIO(i_target, i_mmioAddr, i_transSize, o_data);
#else
    if (fapi2DllFnTable[ECMD_FAPI2GETMMIO] == NULL)
    {
        fapi2DllFnTable[ECMD_FAPI2GETMMIO] = (void*)dlsym(dlHandle, dllFunctionName);
        if (fapi2DllFnTable[ECMD_FAPI2GETMMIO] == NULL)
        {
            fprintf(stderr,"%s%s",dllFunctionName,ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
            ecmdDisplayDllInfo();
            exit(ECMD_DLL_INVALID);
        }
    }

    uint32_t (*Function)(const ecmdChipTarget&, const uint64_t, const size_t, std::vector<uint8_t>&) = 
        (uint32_t(*)(const ecmdChipTarget&, const uint64_t, const size_t, std::vector<uint8_t>&))fapi2DllFnTable[ECMD_FAPI2GETMMIO];
    l_ecmdRc = (*Function)(i_target, i_mmioAddr, i_transSize, o_data);
#endif
    if (l_ecmdRc)
    {
        rc = (fapi2::ReturnCodes) l_ecmdRc; 
    }

#ifndef ECMD_STRIP_DEBUG
    if (ecmdClientDebug != 0)
    {
        args.push_back((void*) &l_ecmdRc);
        ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"getMMIO");
        ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"uint32_t getMMIO(const ecmdChipTarget & i_target, const uint64_t i_mmioAddr, const size_t i_transSize, std::vector<uint8_t>& o_data)",args);
    }
#endif

    return rc;
}

fapi2::ReturnCode putMMIO(const ecmdChipTarget& i_target,
                          const uint64_t i_mmioAddr,
                          const size_t i_transSize,
                          const std::vector<uint8_t>& i_data)
{
    const char * dllFunctionName = "dllFapi2PutMMIO";
    fapi2::ReturnCode rc(fapi2::FAPI2_RC_SUCCESS);
    uint32_t l_ecmdRc;

#ifndef ECMD_STATIC_FUNCTIONS
    if (dlHandle == NULL)
    {
        fprintf(stderr,"%s%s",dllFunctionName,ECMD_DLL_NOT_LOADED_ERROR);
        exit(ECMD_DLL_INVALID);
    }
#endif

    if (!fapi2Initialized)
    {
        fprintf(stderr,"%s: eCMD Extension not initialized before function called\n", dllFunctionName);
        fprintf(stderr,"%s: OR eCMD fapi Extension not supported by plugin\n", dllFunctionName);
        exit(ECMD_DLL_INVALID);
    }

#ifndef ECMD_STRIP_DEBUG
    int myTcount;
    std::vector< void * > args;
    if (ecmdClientDebug != 0)
    {
        args.push_back((void*) &i_target);
        args.push_back((void*) &i_mmioAddr);
        args.push_back((void*) &i_transSize);
        args.push_back((void*) &i_data);
        fppCallCount++;
        myTcount = fppCallCount;
        ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"uint32_t putMMIO(const ecmdChipTarget & i_target, const uint64_t i_mmioAddr, const size_t i_transSize, const std::vector<uint8_t>& i_data)",args);
        ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"putMMIO");
    }
#endif

#ifdef ECMD_STATIC_FUNCTIONS
    l_ecmdRc = dllFapi2PutMMIO(i_target, i_mmioAddr, i_transSize, i_data);
#else
    if (fapi2DllFnTable[ECMD_FAPI2PUTMMIO] == NULL)
    {
        fapi2DllFnTable[ECMD_FAPI2PUTMMIO] = (void*)dlsym(dlHandle, dllFunctionName);
        if (fapi2DllFnTable[ECMD_FAPI2PUTMMIO] == NULL)
        {
            fprintf(stderr,"%s%s",dllFunctionName,ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
            ecmdDisplayDllInfo();
            exit(ECMD_DLL_INVALID);
        }
    }

    uint32_t (*Function)(const ecmdChipTarget&, const uint64_t, const size_t, const std::vector<uint8_t>&) = 
        (uint32_t(*)(const ecmdChipTarget&, const uint64_t, const size_t, const std::vector<uint8_t>&))fapi2DllFnTable[ECMD_FAPI2PUTMMIO];
    l_ecmdRc = (*Function)(i_target, i_mmioAddr, i_transSize, i_data);
#endif
    if (l_ecmdRc)
    {
        rc = (fapi2::ReturnCodes) l_ecmdRc; 
    }

#ifndef ECMD_STRIP_DEBUG
    if (ecmdClientDebug != 0)
    {
        args.push_back((void*) &l_ecmdRc);
        ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"putMMIO");
        ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"uint32_t putMMIO(const ecmdChipTarget & i_target, const uint64_t i_mmioAddr, const size_t i_transSize, std::vector<uint8_t>& i_data)",args);
    }
#endif

    return rc;

}

fapi2::ReturnCode getMMIO(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP, fapi2::plat_target_handle_t>& i_target,
                          const uint64_t i_mmioAddr,
                          const size_t i_transSize,
                          std::vector<uint8_t>& o_data)
{
        ecmdChipTarget ecmdTarget;
        fapiTargetToEcmdTarget(i_target, ecmdTarget);
        return fapi2plat::getMMIO(ecmdTarget, i_mmioAddr, i_transSize, o_data);
}

fapi2::ReturnCode putMMIO(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP, fapi2::plat_target_handle_t>& i_target,
                          const uint64_t i_mmioAddr,
                          const size_t i_transSize,
                          const std::vector<uint8_t>& i_data)
{
        ecmdChipTarget ecmdTarget;
        fapiTargetToEcmdTarget(i_target, ecmdTarget);
        return fapi2plat::putMMIO(ecmdTarget, i_mmioAddr, i_transSize, i_data);
}

};

