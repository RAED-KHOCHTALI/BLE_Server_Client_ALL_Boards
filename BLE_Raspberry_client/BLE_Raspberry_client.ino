from bluepy import btle

MAC = "0c:b8:15:f6:2e:d2"
SERVICE_UUID = "92053ecb-cd1b-4819-aed4-e3e516d14789"
tempCHARACTERISTIC_UUID = "6781f563-0dc3-4a2a-bd58-ad5ce98c6b22"
dataCHARACTERISTIC_UUID = "e39d897c-ebca-473a-87ed-03a6bf9f62c6"

print("Connect to:" + MAC)
dev = btle.Peripheral(MAC)
connect = True

print("\n------------------------------------")
print("Get Serice By UUID: " + SERVICE_UUID)
service_uuid = btle.UUID(SERVICE_UUID)
service = dev.getServiceByUUID(service_uuid)

#----------------------------------------------
characteristics = dev.getCharacteristics()
for char in characteristics:
    if(char.uuid == tempCHARACTERISTIC_UUID ):
        print("=== !tempCHARACTERISTIC_UUID matched! ==")
        temp=char
    if(char.uuid == dataCHARACTERISTIC_UUID):
        print("=== !dataCHARACTERISTIC_UUID matched! ==")
        data = char

while 1 :
    print("type your command:\n")
    x = input()
    if(x=='r'):
            print(temp.read())
    if(x=='s'):
            print("type the data you want to send:  ")
            mystring=input()
            b = bytes(mystring, 'utf-8')
            data.write(b)
    if(x=='d'):
            dev.disconnect()
            connect = False
    if(x=='c'):
        if(connect):
            print("device already connected\n")
            connect = True
        else :
            dev._connect(MAC)
