require "./spec_helper"

describe Crsensor do
  # TODO: Write tests

  it "cpu" do
    cpuInf = Sensors::CPU.new
    cpuInf.type.should eq("CPU")
  end
end
