assume cs:code
data segment  
    dw 7,2,3,9,5,123,456,789,11,22,33,44,55,99,88,77,66,1111,3333,2222,000fh,000eh,000dh,000ch,000bh,000ah,200fh,200eh,200dh,200ch,200bh,200ah,150fh,150eh,150dh,150ch,150bh,150ah,110fh,110eh,110dh,110ch,110bh,110ah,220fh,220eh,220dh,220ch,220bh,220ah
data ends     
code segment 
    
  start:      mov ax ,data
              mov ds ,ax 
              mov si ,0
              mov di ,98  
              call quicksort  
    
              mov ax,4c00h
              int 21h
              
              
              
  partition:  push dx
              push cx
              push es
              mov ax ,di
              sub ax ,si
              mov dx ,0
              mov bx ,2
              div bx
              mov cx ,ax
              mov ax ,si 
              sub ax ,2 
              mov bx ,si
              
            q:push cx
              push bx
              push di
              mov bx,[bx]
              mov di,[di]
              cmp bx, di 
              pop di
              pop bx
              ja l
              add ax ,2
              mov dx ,[bx]
              mov es ,bx
              mov bx ,ax
              mov cx ,[bx] 
              mov [bx] ,dx
              mov bx ,es
              mov [bx] ,cx
            l:add bx ,2
              pop cx
              loop q             
              push bx
              mov bx ,ax
              add bx ,2
              mov dx ,[di]
              push bx
              mov bx ,[bx]
              mov [di] ,bx
              pop bx
              mov [bx] ,dx
              pop bx
              add ax ,2
              pop es
              pop cx
              pop dx
              ret        
                      
  quicksort:  cmp si,di
              jb s
              jmp short ok
            s:call  partition
              mov es ,ax
              mov cx ,si
              mov dx ,di
              push es
              push cx
              push dx
              sub ax ,2 
              mov di ,ax
              call  quicksort
              pop dx
              pop cx
              pop es
              mov ax ,es
              add ax ,2
              mov si ,ax
              mov di ,dx 
              call  quicksort
           ok:ret  
           
           
 
           
code ends
end  start