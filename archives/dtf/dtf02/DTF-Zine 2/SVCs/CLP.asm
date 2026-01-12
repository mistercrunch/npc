
                .386p
                .model flat

        extrn   ExitProcess     :proc
        extrn   GetCommandLineA :proc
        extrn   MessageBoxA     :proc

                .data

        sz_Title        db      'GetCommandLineA',0

                .code

CLP:    call    GetCommandLineA
        mov     edi, eax
        mov     al, 22h
        xor     ecx, ecx
        dec     ecx
        repnz   scasb
        repnz   scasb
        mov     al, 20h
        repnz   scasb

        push    00h
        push    offset sz_Title
        push    edi
        push    00h
        call    MessageBoxA

        push    00h
        call    ExitProcess

end     CLP
end
