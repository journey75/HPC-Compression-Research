import chisel3._

class MyModule extends Module 
{
  val io = IO(new Bundle 
  {
    
  })

  val myArray = RegInit(VecInit(Seq.fill(128)(0.U(32.W))))
  // Initialize a 128-entry array of 32-bit numbers with all zeroes

  val newArray = RegInit(VecInit(Seq.fill(128)(0.U(5.W))))
  // Initialize a 128-entry array of 5-bit numbers with all zeroes

  for (i <- 0 until 128) 
  {
    newArray(i) := myArray(i)(4,0)
    // Take the 5 least significant bits of myArray and assign them to newArray
  }

  // Other code can manipulate myArray and newArray as needed, add later

}
