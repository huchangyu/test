assume cs:code, ds:data
data segment  

    a dw 1 
    b dw 183 
data ends
stack segment
    db   128  dup(0)
stack ends
code segment   
    
 start:jmp init
    
    int9 proc far
    push ax
    push bx
    push cx
    push es
    sti
    pushf
    pushf
    pop ax
    and ah,11111100b
    push ax 
    popf
    call dword ptr cs:[200h]
    
    sti
    mov ah,0
    int 16h
    cli
    cmp al, 61h     
    js second
    cmp al, 7ah
    jns first
    inc al
    
first:
    cmp al, 7ah     
    jne second
    mov al, 61h
second:           
    mov ah, 0ah 
    mov bh, 0
    mov cx, 1
    int 10h
    mov ah, 3 
    mov bh, 0
    int 10h
    inc dl   
    mov ah, 2
    int 10h
    pop es
    pop cx
    pop bx
    pop ax
    iret
  int9 endp
int9end:nop 

int1ch  proc far
    push ax     
    push BX
    push CX
    push DX
    push DS
    STI           
    MOV ax,data
    MOV DS,ax            
    DEC a          
    JNZ NS             
    mov al, 00h
    out 21h, al 
    MOV a, 364             
  NS:
    dec b
    jnz qu
    mov al, 02h
    out 21h, al 
    mov b, 364
    
  qu: 
    CLI            
    pop ds
    pop dx
    pop cx
    pop bx
    pop ax             
    IRET            
int1ch  endp


init:
    mov al, 13H  
    out 20H, al  
    mov al, 8  
    out 21H, al  
    mov al, 8  
    out 21H, al
    mov ax, stack
    mov ss, ax
    mov sp, 128
    push cs
    pop ds
    mov ax, 0
    mov es, ax 
    mov si, offset int9  
    mov di, 204h          
    mov cx, offset int9end - offset int9 
    cld
    rep movsb
    push es:[9*4]       
    pop es:[200h]
    push es:[9*4+2]
    pop es:[202h]
    cli                 
    mov word ptr es:[9*4], 204h
    mov word ptr es:[9*4+2],0
    sti
    mov ax,data
    mov DS,ax           
    mov dx,SEG int1ch   
    mov ds,dx
    lea dx,int1ch           
    
    mov ah,25H                
    mov al,1CH           
    int 21H
    mov ax, 4c00h
    int 21h

  code ends

 end start
