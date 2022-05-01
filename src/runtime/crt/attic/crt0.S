; CRT0 for elfldr crt
; Only linked in for the application version.

; If I wanted reordering, I'd use a high level language.
; </rant>
.set   noat
.set   noreorder

.text
.align   2


.extern main

; Internal CRT symbols
.extern __rt_global_init
.extern __rt_global_fini

.globl  __start
.ent    __start

; Code execution begins here.
__start: