# TODO: Write documentation for `Crsensor`
require "option_parser"

module Crsensor
  VERSION = "0.1.0"
  # Options
  action = ""

  option_parser = OptionParser.parse do |parser|
    parser.on "-a ACTION", "--action=NAME", "Provide a valid action to perform" do |name|
      action = name
    end
  end

  if action == "" 
    p "Nothing to do"
    exit
  end

  @[Link(ldflags: "#{__DIR__}/binsensor.o")]
  @[Link(framework: "IOKit")]
  lib LibApple
    fun SMCGetTemperature(key : LibC::Char*) : Float64
  end

  # TODO: Put your code here
  puts LibApple.SMCGetTemperature("TC0P")

end
