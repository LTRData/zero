#include <ntddk.h>

NTKERNELAPI
ULONG
RtlRandom(IN OUT PULONG Seed);

#ifdef _WIN64

NTKERNELAPI
ULONG
RtlRandomEx(IN OUT PULONG Seed);

#define RtlRandom RtlRandomEx

#endif

PVOID
MmGetSystemAddressForMdlPrettySafe(PMDL Mdl)
{
  CSHORT MdlMappingCanFail;
  PVOID MappedSystemVa;

  if (Mdl == NULL)
    return NULL;

  if (Mdl->MdlFlags & (MDL_MAPPED_TO_SYSTEM_VA | MDL_SOURCE_IS_NONPAGED_POOL))
    return Mdl->MappedSystemVa;

  MdlMappingCanFail = Mdl->MdlFlags & MDL_MAPPING_CAN_FAIL;

  Mdl->MdlFlags |= MDL_MAPPING_CAN_FAIL;

  MappedSystemVa = MmMapLockedPages(Mdl, KernelMode);

  if (MdlMappingCanFail == 0)
    Mdl->MdlFlags &= ~MDL_MAPPING_CAN_FAIL;

  return MappedSystemVa;
}

///////

NTSTATUS
DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath);

NTSTATUS
ZeroDispatchCreateClose(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);

NTSTATUS
ZeroDispatchRead(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);

NTSTATUS
ZeroDispatchWrite(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);

NTSTATUS
ZeroDispatchQueryInformation(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);

NTSTATUS
ZeroDispatchSetInformation(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);

VOID
ZeroUnload(IN PDRIVER_OBJECT DriverObject);

PDEVICE_OBJECT ZeroDev;
PDEVICE_OBJECT RandomDev;
LARGE_INTEGER DriverLoadSystemTime;

#pragma code_seg("INIT")

NTSTATUS
DriverEntry(IN PDRIVER_OBJECT DriverObject,
	    IN PUNICODE_STRING RegistryPath)
{
  NTSTATUS status;
  UNICODE_STRING device_name;
  UNICODE_STRING link_name;

  KeQuerySystemTime(&DriverLoadSystemTime);

  RtlInitUnicodeString(&device_name, L"\\Device\\Zero");

  status = IoCreateDevice(DriverObject, 0, &device_name, FILE_DEVICE_NULL, 0,
			  FALSE, &ZeroDev);

  if (!NT_SUCCESS(status))
    ZeroDev = NULL;
  else
    ZeroDev->Flags |= DO_DIRECT_IO;

  RtlInitUnicodeString(&link_name, L"\\DosDevices\\Zero");
  IoCreateUnprotectedSymbolicLink(&link_name, &device_name);

  RtlInitUnicodeString(&device_name, L"\\Device\\Random");

  status = IoCreateDevice(DriverObject, 0, &device_name, FILE_DEVICE_NULL, 0,
			  FALSE, &RandomDev);

  if (!NT_SUCCESS(status))
    RandomDev = NULL;
  else
    RandomDev->Flags |= DO_DIRECT_IO;

  if ((ZeroDev == NULL) & (RandomDev == NULL))
    return status;

  RtlInitUnicodeString(&link_name, L"\\DosDevices\\Random");
  IoCreateUnprotectedSymbolicLink(&link_name, &device_name);

  DriverObject->MajorFunction[IRP_MJ_CREATE] = ZeroDispatchCreateClose;
  DriverObject->MajorFunction[IRP_MJ_CLOSE] = ZeroDispatchCreateClose;
  DriverObject->MajorFunction[IRP_MJ_READ] = ZeroDispatchRead;
  DriverObject->MajorFunction[IRP_MJ_WRITE] = ZeroDispatchWrite;
  DriverObject->MajorFunction[IRP_MJ_QUERY_INFORMATION] =
    ZeroDispatchQueryInformation;
  DriverObject->MajorFunction[IRP_MJ_SET_INFORMATION] =
    ZeroDispatchSetInformation;

  DriverObject->DriverUnload = ZeroUnload;

  return STATUS_SUCCESS;
}

#pragma code_seg("PAGE")

VOID
ZeroUnload(IN PDRIVER_OBJECT DriverObject)
{
  PDEVICE_OBJECT device_object;
  UNICODE_STRING link_name;

  PAGED_CODE();

  RtlInitUnicodeString(&link_name, L"\\DosDevices\\Zero");
  IoDeleteSymbolicLink(&link_name);
  RtlInitUnicodeString(&link_name, L"\\DosDevices\\Random");
  IoDeleteSymbolicLink(&link_name);

  while ((device_object = DriverObject->DeviceObject) != NULL)
    IoDeleteDevice(device_object);
}

#pragma code_seg()

NTSTATUS
ZeroDispatchCreateClose(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
  Irp->IoStatus.Status = STATUS_SUCCESS;
  Irp->IoStatus.Information = 0;

  IoCompleteRequest(Irp, IO_NO_INCREMENT);

  return STATUS_SUCCESS;
}

NTSTATUS
ZeroDispatchRead(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
  PIO_STACK_LOCATION io_stack = IoGetCurrentIrpStackLocation(Irp);
  PUCHAR system_buffer = MmGetSystemAddressForMdlPrettySafe(Irp->MdlAddress);

  if (system_buffer == NULL)
    {
      Irp->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;
      Irp->IoStatus.Information = 0;

      IoCompleteRequest(Irp, IO_NO_INCREMENT);
	  
      return STATUS_INSUFFICIENT_RESOURCES;
    }

  if (DeviceObject == RandomDev)
    {
      PULONG idx = (PULONG) system_buffer;

      for (;
	   idx <= (PULONG) (system_buffer + io_stack->Parameters.Read.Length -
			    sizeof(ULONG));
	   idx += 1)
	*idx = RtlRandom(&DriverLoadSystemTime.LowPart);
    }
  else // if (DeviceObject == ZeroDev)
    RtlZeroMemory(system_buffer, io_stack->Parameters.Read.Length);

  Irp->IoStatus.Status = STATUS_SUCCESS;
  Irp->IoStatus.Information = io_stack->Parameters.Read.Length;

  IoCompleteRequest(Irp, IO_NO_INCREMENT);

  return STATUS_SUCCESS;
}

NTSTATUS
ZeroDispatchWrite(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
  PIO_STACK_LOCATION io_stack;

  io_stack = IoGetCurrentIrpStackLocation(Irp);

  Irp->IoStatus.Status = STATUS_SUCCESS;
  Irp->IoStatus.Information = io_stack->Parameters.Write.Length;

  IoCompleteRequest(Irp, IO_NO_INCREMENT);

  return STATUS_SUCCESS;
}

NTSTATUS
ZeroDispatchQueryInformation(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
  PIO_STACK_LOCATION io_stack = IoGetCurrentIrpStackLocation(Irp);
  NTSTATUS status;

  switch (io_stack->Parameters.QueryFile.FileInformationClass)
    {
    case FileStandardInformation:
      {
	PFILE_STANDARD_INFORMATION file_standard_information =
	  (PFILE_STANDARD_INFORMATION) Irp->AssociatedIrp.SystemBuffer;

	if (io_stack->Parameters.QueryFile.Length <
	    sizeof(FILE_STANDARD_INFORMATION))
	  {
	    status = STATUS_INVALID_PARAMETER;
	    break;
	  }

	file_standard_information->AllocationSize.QuadPart = 0;
	file_standard_information->EndOfFile.QuadPart = 0;
	file_standard_information->NumberOfLinks = 0;
	file_standard_information->DeletePending = FALSE;
	file_standard_information->Directory = FALSE;

	Irp->IoStatus.Information = sizeof(FILE_STANDARD_INFORMATION);
	status = STATUS_SUCCESS;

	break;
      }

    case FilePositionInformation:
      {
	PFILE_POSITION_INFORMATION file_position_information =
	  (PFILE_POSITION_INFORMATION) Irp->AssociatedIrp.SystemBuffer;

	if (io_stack->Parameters.QueryFile.Length <
	    sizeof(FILE_POSITION_INFORMATION))
	  {
	    status = STATUS_INVALID_PARAMETER;
	    break;
	  }

	file_position_information->CurrentByteOffset.QuadPart = 0;

	Irp->IoStatus.Information = sizeof(FILE_POSITION_INFORMATION);
	status = STATUS_SUCCESS;

	break;
      }

    default:
      status = STATUS_INVALID_DEVICE_REQUEST;
    }

  if (!NT_SUCCESS(status))
    Irp->IoStatus.Information = 0;

  Irp->IoStatus.Status = status;

  IoCompleteRequest(Irp, IO_NO_INCREMENT);

  return status;
}

NTSTATUS
ZeroDispatchSetInformation(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
  PIO_STACK_LOCATION io_stack = IoGetCurrentIrpStackLocation(Irp);
  NTSTATUS status;

  switch (io_stack->Parameters.SetFile.FileInformationClass)
    {
    case FileEndOfFileInformation:
    case FileAllocationInformation:
      status = STATUS_SUCCESS;
      break;

    default:
      status = STATUS_INVALID_DEVICE_REQUEST;
    }

  Irp->IoStatus.Information = 0;
  Irp->IoStatus.Status = status;

  IoCompleteRequest(Irp, IO_NO_INCREMENT);

  return status;
}
