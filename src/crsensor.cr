# TODO: Write documentation for `Crsensor`
module Crsensor
  VERSION = "0.1.0"

  @[Link(ldflags: "#{__DIR__}/binsensor.o")]
  @[Link(framework: "IOKit")]
  lib LibApple
    fun SMCGetTemperature(key : LibC::Char*) : Float64
  end

  # TODO: Put your code here
  puts LibApple.SMCGetTemperature("TC0P")

end
