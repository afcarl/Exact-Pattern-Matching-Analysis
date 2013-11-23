# To change this template, choose Tools | Templates
# and open the template in the editor.
f=open('/home/rapakot/Documents/algorith/algor/src/patterns.txt', 'r')
output1=open('/home/rapakot/Documents/algorith/algor/src/ocurrences.txt', 'w')
d=0
import re
for line in f:
    d=d+1
    print d
    #occ=0
    line = [w.replace('\n', '') for w in line]
    data=''.join(line)
    c=open('/home/rapakot/Documents/algorith/algor/src/text1.txt', 'r')
    for line1 in c:
         matches = re.finditer(r'(?=(%s))' % re.escape(data), line1)
         occ= len([m.start(1) for m in matches])
         #print line1.count(data)
    output1.write(str(d) + '\t'  + str(occ) + '\n')
             
         
f.close()
c.close()
        

