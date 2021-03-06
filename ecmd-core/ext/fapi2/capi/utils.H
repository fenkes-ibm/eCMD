/* IBM_PROLOG_BEGIN_TAG                                                   */
/* 
 * Copyright 2017 IBM International Business Machines Corp.
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
/* IBM_PROLOG_END_TAG                                                     */
/**
 * @file utils.H
 * @brief Defines common fapi2 utilities
 */

#ifndef FAPI2_UTILS_H_
#define FAPI2_UTILS_H_

#include <stdint.h>
#include <return_code.H>
#include <target_types.H>
#include <plat_utils.H>


namespace fapi2
{
#ifndef __PPE__
///
/// @brief Enable/Disable special wakeup on processor chip core(s)
///
/// Special Wakeup Enable must be done when a HWP is doing an operation that
/// requires core(s) to be awake (e.g. modifying the Hcode image). For
/// each Special Wakeup Enable call, there must be a subsequent Special Wakeup
/// Disable call.
///
/// This does not apply to SCOM operations, platforms must handle Special Wakeup
/// for SCOM operations internally.
///
/// If Special Wakeup is enabled, a core will not go to sleep (if already
/// sleeping, it is woken up). If Special Wakeup is disabled, if there are no
/// other active Enables, the core is allowed to sleep.
///
/// @note Implemented by platform code calling the cpu special wakeup HWP.
///       This is a FAPI2 function because each platform may do different things
///         Hostboot: Does nothing (cores cannot sleep while Hostboot running)
///         FSP: Uses an algorithm to decide when to disable special wakeup
///         Cronus: Does Special Wakeup enable/disable as requested
///
/// @param[in] i_target
///              TARGET_TYPE_PROC_CHIP: Enables/Disables Special Wakeup on all
///                cores (EX,EQ chiplets) of the specified chip target.
///              TARGET_TYPE_CORE: Enables/Disables Special Wakeup on the
///                specified core target (EX,EQ chiplets)
///              TARGET_TYPE_EX: Enables/Disables Special Wakeup on the
///                specified EX target.
///              TARGET_TYPE_EQ: Enables/Disables Special Wakeup on the
///                specified EQ target.
///
/// @param[in] i_enable true = enable. false = disable.
///
/// @return ReturnCode. FAPI2_RC_SUCCESS on success, else platform specified error.
///
///
template<TargetType T, typename V>
inline ReturnCode specialWakeup(const Target<T, V>& i_target,
                                const bool i_enable)
{
    // enforce the allowed target types
    static_assert( ((T == fapi2::TARGET_TYPE_PROC_CHIP) ||
                    (T == fapi2::TARGET_TYPE_CORE)      ||
                    (T == fapi2::TARGET_TYPE_EX)        ||
                    (T == fapi2::TARGET_TYPE_EQ)),
                   "Invalid target type for this function");

    ReturnCode l_rc = platSpecialWakeup( i_target, i_enable );

    return l_rc;
}

///
/// @brief Log an error.
///
/// @param[in,out] io_rc Reference to ReturnCode (Any references to data and error
///            target are removed and rc value is set to success after
///            function ends.)
/// @param[in] i_sev Fapi error log severity defaulted to unrecoverable
/// @param[in] i_unitTestError - flag to log error which does not cause a unit
///                              test to fail.
///
/// @note This function is called from the ffdc collection classes and no longer
/// needs to be called directly.
/// @note Implemented by platform code
///
void logError(
    fapi2::ReturnCode& io_rc,
    fapi2::errlSeverity_t i_sev = fapi2::FAPI2_ERRL_SEV_UNRECOVERABLE,
    bool i_unitTestError = false );

///
/// @brief Create a platform error log
///
///  This function will create a platform error log from the passed in
///  return code value and will populate the iv_platDataPtr of the return code
///  with a pointer to the newly created log.
///
/// @param[in,out] io_rc - Reference to ReturnCode
///
/// @param[in] i_sev Fapi error log severity defaulted to unrecoverable
//
//
/// @note Implemented by platform code
///
void createPlatLog(
    fapi2::ReturnCode& io_rc,
    fapi2::errlSeverity_t i_sev = fapi2::FAPI2_ERRL_SEV_UNRECOVERABLE
);

///
/// @brief Associate an error to PRD PLID
///
/// @param[in] i_target Reference to target
/// @param[in,out] io_rc Reference to ReturnCode
/// @param[in] i_sev Fapi error log severity defaulted to unrecoverable
/// @param[in] i_unitTestError - flag to log error which does not cause a unit
///                              test to fail.
///
/// @note Implemented by platform code
///
void log_related_error(
    const Target<TARGET_TYPE_ALL>& i_target,
    fapi2::ReturnCode& io_rc,
    const fapi2::errlSeverity_t i_sev = fapi2::FAPI2_ERRL_SEV_UNRECOVERABLE,
    const bool i_unitTestError = false );

#endif // __PPE__

///
/// @brief Delay this thread. Hostboot will use the nanoseconds parameter
/// and make a syscall to nanosleep. While in the syscall, the hostboot
/// kernel will continue to consume CPU cycles as it looks for a runnable
/// task.  When the delay time expires, the task becomes runnable and will soon
/// return from the syscall.  Callers of delay() in the hostboot environment
/// will likely have to know the mHz clock speed they are running on and
/// compute a non-zero value for i_nanoSeconds.
///
/// On the FSP, it was sometimes acceptable to just provide zero for the
/// sleep delay time, causing the task to yield its time slice. By the
/// time the calling task could run again, it was pretty certain enough
/// host cycles had past.  This is probably not acceptable in
/// the hostboot environment. Callers should calculate and provide a
/// sleep value in nanoseconds relative to host clock speed.
///
/// On FSP when VBU is the target, then the i_simCycles parameter will be
/// used instead.  The FSP needs to use the simdispatcher client/server
/// API and issue a command to the awan to advance the simulation the
/// specified number of cycles.
///
/// @param[in] i_nanoSeconds    nanoseconds to sleep
/// @param[in] i_simCycles      count of Awan cycles to advance
/// @param[in] i_fixed          Determination, for DFT, if this time is
///                             fixed or not. Defaults to non-fixed
///
/// @return ReturnCode. Zero on success, else platform specified error.
///
ReturnCode delay(uint64_t i_nanoSeconds, uint64_t i_simCycles,
                 bool i_fixed = false);

///
/// @brief Assert a condition, and halt
///
/// @param[in] i_expression a boolean representing the assertion
///
void Assert(bool i_expression);
};

#endif // FAPI2_UTILS_H_
