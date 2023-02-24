import chisel3._

class SZFastCompressArgsUnpredictableOneBlockFloat extends Module {
  val io = IO(new Bundle {
    val oriData = Input(Vec(nbEle, UInt(32.W))) //changed from float
    val nbEle = Input(UInt(32.W))
    val absErrBound = Input(UInt(32.W))
    val outputBytes = Output(Vec(outSize, UInt(8.W)))
    val outSize = Output(UInt(32.W))
    val leadNumberArray_int = Output(Vec(nbEle, UInt(8.W)))
    val medianValue = Input(UInt(32.W))
    val radius = Input(UInt(32.W))
  })

  // Your code for SZ_fast_compress_args_unpredictable_one_block_float goes here
}

//-------------------------------------------------------------------------------

val totalSize = Wire(UInt(width = 64.W))
val i = Wire(UInt(width = 64.W))

val reqLength = Wire(UInt(width = 32.W))

//compute median, value range, and radius

//val radExpo = getExponent_float(radius)
val computeReqLengthResult = computeReqLength_float(absErrBound, radExpo)
reqLength := computeReqLengthResult._1
val medianValue = computeReqLengthResult._2

val reqBytesLength = reqLength / 8
val resiBitsLength = reqLength % 8
val rightShiftBits = 0.U
 
 //-----------------------------------------------------------------------------

 val leadNumberArray_size = 
  if (nbEle % 4 == 0) 
    nbEle / 4 
  else 
    nbEle / 4 + 1
  
var lfBuf_pre = BigInt(0)
var lfBuf_cur = BigInt(0)

val leadNumberArray = outputBytes.slice(1 + 4, 1 + 4 + leadNumberArray_size)

val exactMidbyteArray = leadNumberArray + leadNumberArray_size

if (resiBitsLength != 0) {
  rightShiftBits = 8 - resiBitsLength
  reqBytesLength += 1
}
