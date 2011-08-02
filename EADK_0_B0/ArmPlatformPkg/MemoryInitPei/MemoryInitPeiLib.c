/** @file
*
*  Copyright (c) 2011, ARM Limited. All rights reserved.
*
*  This program and the accompanying materials
*  are licensed and made available under the terms and conditions of the BSD License
*  which accompanies this distribution.  The full text of the license may be found at
*  http://opensource.org/licenses/bsd-license.php
*
*  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
*  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
*
**/

#include <PiPei.h>

#include <Library/ArmPlatformLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>

#include <Chipset/ArmV7.h>

VOID
BuildMemoryTypeInformationHob (
  VOID
  );

VOID
InitMmu (
  VOID
  )
{
  ARM_MEMORY_REGION_DESCRIPTOR  *MemoryTable;
  VOID                          *TranslationTableBase;
  UINTN                         TranslationTableSize;

  // Get Virtual Memory Map from the Platform Library
  ArmPlatformGetVirtualMemoryMap(&MemoryTable);

  //Note: Because we called PeiServicesInstallPeiMemory() before to call InitMmu() the MMU Page Table resides in
  //      DRAM (even at the top of DRAM as it is the first permanent memory allocation)
  ArmConfigureMmu (MemoryTable, &TranslationTableBase, &TranslationTableSize);
}

/*++

Routine Description:



Arguments:

  FileHandle  - Handle of the file being invoked.
  PeiServices - Describes the list of possible PEI Services.

Returns:

  Status -  EFI_SUCCESS if the boot mode could be set

--*/
EFI_STATUS
EFIAPI
MemoryPeim (
  IN EFI_PHYSICAL_ADDRESS               UefiMemoryBase,
  IN UINT64                             UefiMemorySize
  )
{
  EFI_RESOURCE_ATTRIBUTE_TYPE ResourceAttributes;
  UINT64                      ResourceLength;
  EFI_PEI_HOB_POINTERS        NextHob;
  EFI_PHYSICAL_ADDRESS        FdTop;
  EFI_PHYSICAL_ADDRESS        SystemMemoryTop;
  EFI_PHYSICAL_ADDRESS        ResourceTop;
  BOOLEAN                     Found;

  // Ensure PcdSystemMemorySize has been set
  ASSERT (PcdGet32 (PcdSystemMemorySize) != 0);

  //
  // Now, the permanent memory has been installed, we can call AllocatePages()
  //
  ResourceAttributes = (
      EFI_RESOURCE_ATTRIBUTE_PRESENT |
      EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
      EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE |
      EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |
      EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
      EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE |
      EFI_RESOURCE_ATTRIBUTE_TESTED
  );

  // Reserved the memory space occupied by the firmware volume
  BuildResourceDescriptorHob (
      EFI_RESOURCE_SYSTEM_MEMORY,
      ResourceAttributes,
      PcdGet32 (PcdSystemMemoryBase),
      PcdGet32 (PcdSystemMemorySize)
  );

  SystemMemoryTop = PcdGet32 (PcdSystemMemoryBase) + PcdGet32 (PcdSystemMemorySize);
  FdTop = PcdGet32(PcdNormalFdBaseAddress) + PcdGet32(PcdNormalFdSize);

  // EDK2 does not have the concept of boot firmware copied into DRAM. To avoid the DXE
  // core to overwrite this area we must mark the region with the attribute non-present
  if ((PcdGet32 (PcdNormalFdBaseAddress) >= PcdGet32 (PcdSystemMemoryBase)) && (FdTop <= SystemMemoryTop)) {
    Found = FALSE;

    // Search for System Memory Hob that contains the firmware
    NextHob.Raw = GetHobList ();
    while ((NextHob.Raw = GetNextHob (EFI_HOB_TYPE_RESOURCE_DESCRIPTOR, NextHob.Raw)) != NULL) {
      if ((NextHob.ResourceDescriptor->ResourceType == EFI_RESOURCE_SYSTEM_MEMORY) &&
          (PcdGet32(PcdNormalFdBaseAddress) >= NextHob.ResourceDescriptor->PhysicalStart) &&
          (FdTop <= NextHob.ResourceDescriptor->PhysicalStart + NextHob.ResourceDescriptor->ResourceLength))
      {
        ResourceAttributes = NextHob.ResourceDescriptor->ResourceAttribute;
        ResourceLength = NextHob.ResourceDescriptor->ResourceLength;
        ResourceTop = NextHob.ResourceDescriptor->PhysicalStart + ResourceLength;

        if (PcdGet32(PcdNormalFdBaseAddress) == NextHob.ResourceDescriptor->PhysicalStart) {
          if (SystemMemoryTop == FdTop) {
            NextHob.ResourceDescriptor->ResourceAttribute = ResourceAttributes & ~EFI_RESOURCE_ATTRIBUTE_PRESENT;
          } else {
            // Create the System Memory HOB for the firmware with the non-present attribute
            BuildResourceDescriptorHob (EFI_RESOURCE_SYSTEM_MEMORY,
                                        ResourceAttributes & ~EFI_RESOURCE_ATTRIBUTE_PRESENT,
                                        PcdGet32(PcdNormalFdBaseAddress),
                                        PcdGet32(PcdNormalFdSize));

            // Top of the FD is system memory available for UEFI
            NextHob.ResourceDescriptor->PhysicalStart += PcdGet32(PcdNormalFdSize);
            NextHob.ResourceDescriptor->ResourceLength -= PcdGet32(PcdNormalFdSize);
          }
        } else {
          // Create the System Memory HOB for the firmware with the non-present attribute
          BuildResourceDescriptorHob (EFI_RESOURCE_SYSTEM_MEMORY,
                                      ResourceAttributes & ~EFI_RESOURCE_ATTRIBUTE_PRESENT,
                                      PcdGet32(PcdNormalFdBaseAddress),
                                      PcdGet32(PcdNormalFdSize));

          // Update the HOB
          NextHob.ResourceDescriptor->ResourceLength = PcdGet32(PcdNormalFdBaseAddress) - NextHob.ResourceDescriptor->PhysicalStart;

          // If there is some memory available on the top of the FD then create a HOB
          if (FdTop < NextHob.ResourceDescriptor->PhysicalStart + ResourceLength) {
            // Create the System Memory HOB for the remaining region (top of the FD)
            BuildResourceDescriptorHob (EFI_RESOURCE_SYSTEM_MEMORY,
                                        ResourceAttributes,
                                        FdTop,
                                        ResourceTop - FdTop);
          }
        }
        Found = TRUE;
        break;
      }
      NextHob.Raw = GET_NEXT_HOB (NextHob);
    }

    ASSERT(Found);
  }

  // Build Memory Allocation Hob
  InitMmu ();

  if (FeaturePcdGet (PcdPrePiProduceMemoryTypeInformationHob)) {
    // Optional feature that helps prevent EFI memory map fragmentation.
    BuildMemoryTypeInformationHob ();
  }

  return EFI_SUCCESS;
}
