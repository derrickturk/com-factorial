Option Explicit

Public Sub UseFactorial()
    Dim f As IFactorial
    
    Set f = CreateObject("CppCom.Factorial")
    
    Debug.Print f.Factorial32(5)
    Debug.Print f.Factorial16(5)
End Sub
