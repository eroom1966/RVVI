/*
 *
 * Copyright (c) 2005-2022 Imperas Software Ltd., www.imperas.com
 *
 * The contents of this file are provided under the Software License Agreement
 * that you accepted before downloading this file.
 *
 * This header forms part of the Software but may be included and used unaltered
 * in derivative works.
 *
 * For more information, please visit www.OVPworld.org or www.imperas.com
 */

#pragma once

/*! \file rvvi.h
 *  \brief RVVI interface, C API header.
**/

#include <stdint.h>

typedef uint32_t bool_t;

#define RVVI_FALSE          0
#define RVVI_TRUE           1
#define RVVI_INVALID_INDEX -1

/*! \brief RVVI API version.
 *
 *  \note This should be passed into the rvviCheckVersion() function.
 */
#define RVVI_API_VERSION 5

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Check the compiled RVVI API version.
 *
 * Makes sure the linked implementation matches the versions defined in this
 * header file. This should be called before any other RVVI API function.
 *
 *  \param version Should be set to RVVI_API_VERSION.
 *
 *  \return returns RVVI_TRUE if versions matches otherwise RVVI_FALSE.
**/
extern bool_t rvviVersionCheck(
    uint32_t version);

/*! \brief Initialize reference.
 *
 *  \param programPath Path to the ELF file the reference model will execute.
 *                     This parameter can be NULL.
 *  \param vendor      Vendor string that the reference model will use.
 *  \param variant     Variant string that the reference model will use.
 *
 *  \return Returns RVVI_TRUE if the reference was initialized successfully else
 *          RVVI_FALSE.
 *
 *  \note The reference model will begin execution from the entry point of the
 *        provided ELF file by the programPath parameter. This can however be
 *        overridden by the rvviRefPcSet() function.
**/
extern bool_t rvviRefInit(
    const char *programPath,
    const char *vendor,
    const char *variant);

/*! \brief Force the reference PC to be particular value.
 *
 *  \param hartId  The hart to change the PC register of.
 *  \param address The address to change the PC register to.
 *
 *  \return Returns RVVI_TRUE if the operation was successful else RVVI_FALSE.
**/
extern bool_t rvviRefPcSet(
    uint32_t hartId,
    uint64_t address);

/*! \brief Shutdown the reference module releasing any used resources.
 *
 *  \return Returns RVVI_TRUE if shutdown was successful else RVVI_FALSE.
**/
extern bool_t rvviRefShutdown(void);

/*! \brief Notify the reference that a CSR is considered volatile.
 *
 *  \param hartId The hart that has updated its GPR.
 *  \param index  Index of the CSR register to be considered volatile
 *                (0x0 to 0xfff).
 *
 *  \return Returns RVVI_TRUE if operation was successful else RVVI_FALSE.
**/
extern bool_t rvviRefCsrSetVolatile(
    uint32_t hartId,
    uint32_t index);

/*! \brief Lookup a net on the reference model and return its index.
 *
 *  \param name The net name to locate.
 *
 *  \return Unique index for this net or RVVI_INVALID_INDEX if it was not found.
 *
 *  \note Please consult the model datasheet for a list of valid net names.
 *
 *  \sa rvviRefNetSet()
**/
extern uint64_t rvviRefNetIndexGet(
    const char *name);

/*! \brief Notify RVVI that a DUT RVV Vector register has been written to.
 *
 *  \param hartId The hart that has updated its FPR.
 *  \param index  The FPR index within the register file (0 to 31).
 *  \param data   Memory to copy the vector register from.
 *  \param size   Size of the memory buffer data parameter in bytes.
**/
extern void rvviDutVrSet(
    uint32_t  hartId,
    uint32_t  index,
    void     *data,
    uint32_t  size);

/*! \brief Notify RVVI that a DUT floating point register has been written to.
 *
 *  \param hartId The hart that has updated its FPR.
 *  \param index  The FPR index within the register file (0 to 31).
 *  \param value  The value that has been written.
**/
extern void rvviDutFprSet(
    uint32_t hartId,
    uint32_t index,
    uint64_t value);

/*! \brief Notify RVVI that a DUT GPR has been written to.
 *
 *  \param hartId The hart that has updated its GPR.
 *  \param index  The GPR index within the register file.
 *  \param value  The value that has been written.
**/
extern void rvviDutGprSet(
    uint32_t hartId,
    uint32_t index,
    uint64_t value);

/*! \brief Notify RVVI that a DUT CSR has been written to.
 *
 *  \param hartId The hart that has updated its CSR.
 *  \param index  The CSR index (0x0 to 0xfff).
 *  \param value  The value that has been written.
**/
extern void rvviDutCsrSet(
    uint32_t hartId,
    uint32_t index,
    uint64_t value);

/*! \brief Propagate a net change to the reference model.
 *
 *  \param index The net index returned prior by rvviRefNetIndexGet().
 *  \param value The new value to set the net state to.
 *
 *  \sa rvviRefNetIndexGet()
**/
extern void rvviRefNetSet(
    uint64_t index,
    uint64_t value);

/*! \brief Notify the reference that a DUT instruction has retired.
 *
 * During execution of a rvviDutStep() an instruction has been retired from
 * the DUT pipeline.
 *
 * If an instruction was unable to retire due to an exception rvviRefException()
 * may be issued instead.
 *
 *  \param hartId    The hart that has retired an instruction.
 *  \param dutPc     The address of the instruction that has retired.
 *  \param dutInsBin The binary instruction representation.
 *
 *  \sa rvviDutException()
**/
extern void rvviDutRetire(
    uint32_t hartId,
    uint64_t dutPc,
    uint64_t dutInsBin);

/*! \brief Notify the reference that the DUT raised an exception.
 *
 *  \param hartId The hart that has received the exception.
 *  \param dutPc  The address of the faulting instruction.
 *
 *  \sa rvviRefOnRetire()
**/
extern void rvviDutException(
    uint32_t hartId,
    uint64_t dutPc);

/*! \brief Step the reference model until the next event.
 *
 *  \param hartId The ID of the hart that is being stepped.
 *
 *  \return Returns RVVI_TRUE if the step was successful else RVVI_FALSE.
**/
extern bool_t rvviRefEventStep(uint32_t hartId);

/*! \brief Compare all GPR register values between reference and DUT.
 *
 *  \param hartId The ID of the hart that is being compared.
 *
 *  \return RVVI_FALSE if there are any mismatches, otherwise RVVI_TRUE.
**/
extern bool_t rvviRefGprsCompare(uint32_t hartId);

/*! \brief Compare GPR registers that have been written to between the reference
 *         and DUT. This can be seen as a super set of the rvviRefGprsCompare
 *         function. This comparator will also flag differences in the set of
 *         registers that have been written to.
 *
 *  \param hartId   The ID of the hart that is being compared.
 *  \param ignoreX0 RVVI_TRUE to not compare writes to the x0 register, which
 *                  may be treated as a special case, otherwise RVVI_FALSE.
 *
 *  \return RVVI_FALSE if there are any mismatches, otherwise RVVI_TRUE.
**/
extern bool_t rvviRefGprsCompareWritten(
    uint32_t hartId,
    bool_t   ignoreX0);

/*! \brief Compare retired instruction bytes between reference and DUT.
 *
 *  \param hartId The ID of the hart that is being compared.
 *
 *  \return RVVI_FALSE if there are any mismatches, otherwise RVVI_TRUE.
**/
extern bool_t rvviRefInsBinCompare(uint32_t hartId);

/*! \brief Compare program counter for the retired instructions between DUT and
 *         the the reference model.
 *
 *  \param hartId The ID of the hart that is being compared.
 *
 *  \return RVVI_FALSE if there are any mismatches, otherwise RVVI_TRUE.
**/
extern bool_t rvviRefPcCompare(uint32_t hartId);

/*! \brief Compare CSRs values between DUT and the the reference model.
 *
 *  \param hartId The ID of the hart that is being compared.
 *
 *  \return RVVI_FALSE if there are any mismatches, otherwise RVVI_TRUE.
**/
extern bool_t rvviRefCsrsCompare(uint32_t hartId);

/*! \brief Compare all RVV vector register values between reference and DUT.
 *
 *  \param hartId The ID of the hart that is being compared.
 *
 *  \return RVVI_FALSE if there are any mismatches, otherwise RVVI_TRUE.
**/
extern bool_t rvviRefVrsCompare(uint32_t hartId);

/*! \brief Compare all floating point register values between reference and DUT.
 *
 *  \param hartId The ID of the hart that is being compared.
 *
 *  \return RVVI_FALSE if there are any mismatches, otherwise RVVI_TRUE.
**/
extern bool_t rvviRefFprsCompare(uint32_t hartId);

/*! \brief Read a GPR value from a hart in the reference model.
 *
 *  \param hartId The hart to retrieve the GPR of.
 *  \param index  Index of the GPR register to read.
 *
 *  \return The GPR register value read from the specified hart.
**/
extern uint64_t rvviRefGprGet(
    uint32_t hartId,
    uint32_t index);

/*! \brief Return the program counter of a hart in the reference model.
 *
 *  \param hartId The hart to retrieve the PC of.
 *
 *  \return The program counter of the specified hart.
**/
extern uint64_t rvviRefPcGet(uint32_t hartId);

/*! \brief Read a CSR value from a hart in the reference model.
 *
 *  \param hartId The hart to retrieve the CSR of.
 *  \param index  Index of the CSR register to read (0x0 to 0xfff).
 *
 *  \return The CSR register value read from the specified hart.
**/
extern uint64_t rvviRefCsrGet(
    uint32_t hartId,
    uint32_t index);

/*! \brief Return the binary representation of the previously retired
 *         instruction.
 *
 *  \param hartId The hart to retrieve the instruction from.
 *
 *  \return The instruction bytes.
**/
extern uint64_t rvviRefInsBinGet(uint32_t hartId);

/*! \brief Read a floating point register value from a hart in the reference
 *         model.
 *
 *  \param hartId The hart to retrieve the register from.
 *  \param index  Index of the floating point register to read.
 *
 *  \return The GPR register value read from the specified hart.
**/
extern uint64_t rvviRefFprGet(
    uint32_t hartId,
    uint32_t index);

/*! \brief Read a RVV vector register value from a hart in the reference model.
 *
 *  \param hartId The hart to retrieve the register from.
 *  \param index  Index of the floating point register to read.
 *  \param data   Pointer to memory that the vector regsiter will be written to.
 *  \param size   Size of the memory region pointed to by data.
**/
extern void rvviRefVrGet(
    uint32_t  hartId,
    uint32_t  index,
    void     *data,
    uint32_t  size);

/*! \brief Notify RVVI that the DUT has been written to memory.
 *
 *  \param hartId         The hart that issued the data bus write.
 *  \param address        The address the hart is writing to.
 *  \param value          The value placed on the data bus.
 *  \param byteEnableMask The byte enable mask provided for this write.
 *
 *  \note Bus writes larger than 64bits should be reported using multiple
 *        calls to this function.
 *  \note byteEnableMask bit 0 corresponds to address+0, bEnMask bit 1
 *        corresponds to address+1, etc.
**/
extern void rvviDutBusWrite(
    uint32_t hartId,
    uint64_t address,
    uint64_t value,
    uint32_t byteEnableMask);

/*! \brief Write data to the reference models physical memory space.
 *
 *  \param hartId  The hart to write from the perspective of.
 *  \param address The address being written to.
 *  \param data    The data byte being written into memory.
 *  \param size    Size of the data being written in bytes (1 to 8).
**/
extern void rvviRefMemoryWrite(
    uint32_t hartId,
    uint64_t address,
    uint64_t data,
    uint32_t size);

/*! \brief Read data from the reference models physical memory space.
 *
 *  \param hartId  The hart to read from the perspective of.
 *  \param address The address being read from.
 *  \param size    Size of the data being written in bytes (1 to 8).
**/
extern uint64_t rvviRefMemoryRead(
    uint32_t hartId,
    uint64_t address,
    uint32_t size);

#ifdef __cplusplus
}  // extern "C"
#endif
