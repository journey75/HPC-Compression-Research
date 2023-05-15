import chisel3._

class PrecisionReqLengthFloat extends Module {
  val io = IO(new Bundle {
    val precision = Input(UInt(32.W))
    val precisionReqLength = Output(UInt(16.W))
  })

  val lbuf = Wire(new LFloat())
  lbuf.value := io.precision

  val ivalue = lbuf.ivalue

  val expValue = (ivalue & 0x7F800000.U) >> 23.U
  val expValueMinus127 = expValue - 127.U

  io.precisionReqLength := expValueMinus127(15, 0)
}

class LFloat extends Bundle {
  val ivalue = UInt(32.W)
}

object PrecisionReqLengthFloat extends App {
  chisel3.Driver.execute(args, () => new PrecisionReqLengthFloat())
}

//-----------------------------------------------------------------------------------------------

import chisel3._

class ComputeReqLengthFloat extends Module {
  val io = IO(new Bundle {
    val realPrecision = Input(UInt(64.W))
    val radExpo = Input(SInt(16.W))
    val reqLength = Output(UInt(32.W))
    val medianValue = Output(UInt(32.W))
  })

  val reqExpo = Wire(UInt(16.W))
  reqExpo := getPrecisionReqLengthDouble(io.realPrecision)
  //look up getreqlengthdouble and program it

  val reqMantiLength = io.radExpo - SInt(reqExpo)

  io.reqLength := (9.U + io.radExpo.asUInt() - reqExpo.asUInt() + 1.U).asUInt()

  when(io.reqLength < 9.U) {
    io.reqLength := 9.U
  }

  when(io.reqLength > 32.U) {
    io.reqLength := 32.U
    io.medianValue := 0.U
  }
}

class LDouble extends Bundle {
  val ivalue = UInt(64.W)
}

object ComputeReqLengthFloat extends App {
  chisel3.Driver.execute(args, () => new ComputeReqLengthFloat())
}

//---------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------
// main compression function

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
}

//--------------------------------------

val totalSize = Wire(UInt(width = 64.W))
val i = Wire(UInt(width = 64.W))

val reqLength = Wire(UInt(width = 32.W))

//compute median, value range, and radius

val radExpo = getExponent_float(radius)
val computeReqLengthResult = computeReqLength_float(absErrBound, radExpo)
reqLength := computeReqLengthResult._1
val medianValue = computeReqLengthResult._2

val reqBytesLength = reqLength / 8
val resiBitsLength = reqLength % 8
val rightShiftBits = 0.U
 
 //------------------------------------

 val leadNumberArray_size = 
  if (nbEle % 4 == 0) 
    nbEle / 4 
  else 
    nbEle / 4 + 1
  
var lfBuf_pre = BigInt(0) //32 bit uints
var lfBuf_cur = BigInt(0) //32 bit uints

val leadNumberArray = outputBytes.slice(1 + 4, 1 + 4 + leadNumberArray_size)

val exactMidbyteArray = leadNumberArray + leadNumberArray_size

if (resiBitsLength != 0) {
  rightShiftBits = 8 - resiBitsLength
  reqBytesLength += 1
}



//---------------------begin the variable portion
//starting with reqbytes length  = 2
UInt8 leadingNum = 0.U
var residualMidBytes_size = 0.U

//here i am zeroing out prebuf because before the loop starts, it could contain arbitrary data, is that ok?
//val lfBuf_pre = Wire(new Uint())
lfBuf_pre.value := 0.U

when (sysEndianType === LITTLE_ENDIAN_SYSTEM) {
    when (reqBytesLength === 2.U) {
        for (i <- 0 until nbEle) {

            leadingNum = 0.U

            lfBuf_cur.value := oriData(i) //get rid of value and ivalue

            lfBuf_cur.ivalue := lfBuf_cur.ivalue >> rightShiftBits

            lfBuf_pre.ivalue := lfBuf_cur.ivalue ^ lfBuf_pre.ivalue

            when (lfBuf_pre.ivalue >> 8.U === 0.U) {
                leadingNum = 3.U
            } .elsewhen (lfBuf_pre.ivalue >> 16.U === 0.U) {
                leadingNum = 2.U
            } .elsewhen (lfBuf_pre.ivalue >> 24.U === 0.U) {
                leadingNum = 1.U
            }

            leadNumberArray_int(i) := leadingNum

            when (leadingNum === 0.U) {
                exactMidbyteArray(residualMidBytes_size) := lfBuf_cur.byte(2) //either 8 to 16 or 24 to 16, grab the second group of 8 bits
                exactMidbyteArray(residualMidBytes_size + 1.U) := lfBuf_cur.byte(3)
                residualMidBytes_size += 2.U
            } .elsewhen (leadingNum === 1.U) {
                exactMidbyteArray(residualMidBytes_size) := lfBuf_cur.byte(2)
                residualMidbyte_size += 1.U
            }

            lfBuf_pre := lfBuf_cur
        }
    } .elsewhen (reqBytesLength === 3.U) {
        for (i <- 0 until nbEle) {

            leadingNum = 0.U

            lfBuf_cur.value := oriData(i) //get rid of value and ivalue

            lfBuf_cur.ivalue := lfBuf_cur.ivalue >> rightShiftBits

            lfBuf_pre.ivalue := lfBuf_cur.ivalue ^ lfBuf_pre.ivalue

            when (lfBuf_pre.ivalue >> 8.U === 0.U) {
                leadingNum = 3.U
            } .elsewhen (lfBuf_pre.ivalue >> 16.U === 0.U) {
                leadingNum = 2.U
            } .elsewhen (lfBuf_pre.ivalue >> 24.U === 0.U) {
                leadingNum = 1.U
            }

            leadNumberArray_int(i) := leadingNum

            when (leadingNum === 0.U) {
                exactMidbyteArray(residualMidBytes_size) := lfBuf_cur.byte(1) //either 8 to 16 or 24 to 16, grab the second group of 8 bits
                exactMidbyteArray(residualMidBytes_size + 1.U) := lfBuf_cur.byte(2)
                exactMidbyteArray(residualMidBytes_size + 1.U) := lfBuf_cur.byte(3)
                residualMidBytes_size += 3.U
            } .elsewhen (leadingNum === 1.U) {
                exactMidbyteArray(residualMidBytes_size) := lfBuf_cur.byte(1)
                exactMidbyteArray(residualMidBytes_size + 1.U) := lfBuf_cur.byte(2)
                residualMidbyte_size += 2.U
            } .elsewhen (leadingNum === 2.U) {
                exactMidbyteArray(residualMidBytes_size) := lfBuf_cur.byte(1)
                residualMidbyte_size += 1.U
            }

            lfBuf_pre := lfBuf_cur
        }

    } .elsewhen (reqBytesLength === 1.U) {
        for (i <- 0 until nbEle) {

            leadingNum = 0.U

            lfBuf_cur.value := oriData(i) //get rid of value and ivalue

            lfBuf_cur.ivalue := lfBuf_cur.ivalue >> rightShiftBits

            lfBuf_pre.ivalue := lfBuf_cur.ivalue ^ lfBuf_pre.ivalue

            when (lfBuf_pre.ivalue >> 8.U === 0.U) {
                leadingNum = 3.U
            } .elsewhen (lfBuf_pre.ivalue >> 16.U === 0.U) {
                leadingNum = 2.U
            } .elsewhen (lfBuf_pre.ivalue >> 24.U === 0.U) {
                leadingNum = 1.U
            }

            leadNumberArray_int(i) := leadingNum

            when (leadingNum === 0.U) {
                exactMidbyteArray(residualMidBytes_size) := lfBuf_cur.byte(3) //either 8 to 16 or 24 to 16, grab the second group of 8 bits
                residualMidBytes_size += 1.U
            } 

            lfBuf_pre := lfBuf_cur
        }

    } .elsewhen (reqBytesLength === 4.U) {
        for (i <- 0 until nbEle) {

            leadingNum = 0.U

            lfBuf_cur.value := oriData(i) //get rid of value and ivalue

            lfBuf_cur.ivalue := lfBuf_cur.ivalue >> rightShiftBits

            lfBuf_pre.ivalue := lfBuf_cur.ivalue ^ lfBuf_pre.ivalue

            when (lfBuf_pre.ivalue >> 8.U === 0.U) {
                leadingNum = 3.U
            } .elsewhen (lfBuf_pre.ivalue >> 16.U === 0.U) {
                leadingNum = 2.U
            } .elsewhen (lfBuf_pre.ivalue >> 24.U === 0.U) {
                leadingNum = 1.U
            }

            leadNumberArray_int(i) := leadingNum

            when (leadingNum === 0.U) {
                exactMidbyteArray(residualMidBytes_size) := lfBuf_cur.byte(0) //either 8 to 16 or 24 to 16, grab the second group of 8 bits
                exactMidbyteArray(residualMidBytes_size + 1.U) := lfBuf_cur.byte(1)
                exactMidbyteArray(residualMidBytes_size + 1.U) := lfBuf_cur.byte(2)
                exactMidbyteArray(residualMidBytes_size + 1.U) := lfBuf_cur.byte(3)
                residualMidBytes_size += 4.U
            } .elsewhen (leadingNum === 1.U) {
                exactMidbyteArray(residualMidBytes_size) := lfBuf_cur.byte(0)
                exactMidbyteArray(residualMidBytes_size + 1.U) := lfBuf_cur.byte(1)
                exactMidbyteArray(residualMidBytes_size + 1.U) := lfBuf_cur.byte(2)
                residualMidbyte_size += 3.U
            } .elsewhen (leadingNum === 2.U) {
                exactMidbyteArray(residualMidBytes_size) := lfBuf_cur.byte(0)
                exactMidbyteArray(residualMidBytes_size + 1.U) := lfBuf_cur.byte(1)
                residualMidbyte_size += 2.U
            } .elsewhen (leadingNum === 3.U) {
                exactMidbyteArray(residualMidBytes_size) := lfBuf_cur.byte(0)
                residualMidbyte_size += 1.U
            }

            lfBuf_pre := lfBuf_cur
        }

    }

} .elsewhen () {

}

//*outSize = totalSize;



--------------------------------------------------------------------------------------------------------------------------------------------
//this is testing related code
import chisel3._
import chisel3.iotesters._

class PrecisionReqLengthFloatTester(dut: PrecisionReqLengthFloat) extends PeekPokeTester(dut) {
  poke(dut.io.precision, 1.U)

  expect(dut.io.precisionReqLength, 0.U)

  val output = peek(dut.io.precisionReqLength)
  println(s"precisionReqLength: ${output.toString(2)}")
}

// run the test
object PrecisionReqLengthFloatTester extends App {
  chisel3.iotesters.Driver(() => new PrecisionReqLengthFloat) { dut =>
    new PrecisionReqLengthFloatTester(dut)
  }
}

sbt "test:runMain PrecisionReqLengthFloatTester"
