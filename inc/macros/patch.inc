
%macro @PATCH 1
    %ifctx __patch
        %error "can't nest patches"
    %else
        [section .patch]
        %push __patch
        dd (%1)                 ; patch addrss
        dd %$end - %$start      ; length of the patch
      %$start:
    %endif
%endmacro

%macro @ENDPATCH 0
    %ifctx __patch
      %$end:
        %pop
        __SECT__
    %else
        %error "no corresponding `@PATCH'"
    %endif
%endmacro

%macro @SET 2
    @PATCH (%1)
    %2
    @ENDPATCH
%endmacro

;; arg1 : start address, will clear this
;; arg2 : what byte to put here. but be a byte long or shit breaks
;; arg3 : stop address, will NOT clear this.
%macro @CLEAR 3
    @PATCH (%1)
    %if ((%3) - (%1)) < 0
        %error "stop address is less than start address in @CLEAR"
    %endif
    times ((%3) - (%1)) db (%2) ; patch data
    @ENDPATCH
%endmacro

%macro @SJMP 2
    @PATCH (%1)
    db 0xEB                     ; short jump
    db (%2) - (%1) - 2          ; relative offset
    @ENDPATCH
%endmacro

%macro @LJMP 2
    @PATCH (%1)
    db 0xE9                     ; far jump
    dd (%2) - (%1) - 5          ; relative offset
    @ENDPATCH
%endmacro

%macro @CALL 2
    @PATCH (%1)
    db 0xE8                     ; far call
    dd (%2) - (%1) - 5          ; relative offset
    @ENDPATCH
%endmacro

%macro @CALL_NOP 2
    @PATCH (%1)
    db 0xE8                     ; far call
    dd (%2) - (%1) - 5          ; relative offset
    db 0x90                     ; NOP
    @ENDPATCH
%endmacro

%macro @HOOK 1
    %define @HOOKEND (%1 + 5)
    @LJMP {(%1)}, dest%1
    [section .text]
    align 16, db 0xCC
    dest%1:
%endmacro

%macro @HOOK 2
    %define @HOOKEND %2
    %if %2 - ((%1) + 5) < 0
        %error "end must be at least 5 bytes (the size of a long jump) after start"
    %endif

    @CLEAR {(%1) + 5}, 0xCC, {(%2)}
    @LJMP {(%1)}, dest%1
    [section .text]
    align 16, db 0xCC
    dest%1:
%endmacro

%macro @DETOUR 3
    cglobal %1
    %define @DETOUREND (%3 + 5)
    @LJMP {(%3)}, %2
    [section .text]
    align 16, db 0xCC
    %1:
%endmacro

%macro @DETOUR 4
    cglobal %1
    %define @DETOUREND %4
    %if %4 - ((%3) + 5) < 0
        %error "end must be at least 5 bytes (the size of a long jump) after start"
    %endif

    @CLEAR {(%3) + 5}, 0xCC, {(%4)}
    @LJMP {(%3)}, %2
    [section .text]
    align 16, db 0xCC
    %1:
%endmacro

%macro @CLEAR_NOP 2
    @CLEAR %1, 0x90, %2
%endmacro

%macro @CLEAR_INT 2
    @CLEAR %1, 0xCC, %2
%endmacro

%macro @LJMP_NOP 3
    @CLEAR_NOP {(%1) + 5}, %2
    @LJMP {(%1)}, %3
%endmacro

%macro @LJMP_INT 3
    @CLEAR_INT {(%1) + 5}, %2
    @LJMP {(%1)}, %3
%endmacro
