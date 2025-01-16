start: ldc array 
       a2sp              
       ldl 0   
       stl 5           
       ldc 1    
       stl 7                

loop: ldc 5    
      ldl 7 
      sub
      brlz done

      ldl 7   
      ldc array 
      add
      ldnl 0

      ldl 5
      sub
      brlz skip

      ldl 7       
      ldc array
      add
      ldnl 0
      stl 5

skip: ldl 7                
      ldc 1 
      add 
      stl 7
      br loop              

done: ldl 5 

HALT        

array: data 1         
       data 2            
       data 3         
       data 4   
       data 5 

result:data 0 
