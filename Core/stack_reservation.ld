/* Preserve 2 KiB below the stack top without editing SysConfig output. */
__StackLimit = __StackTop - 0x800;
ASSERT(__heap_start__ <= __StackLimit, "Static data overlaps the reserved stack")
