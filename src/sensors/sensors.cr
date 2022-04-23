module Sensors
    abstract class Sensor(T)
        abstract def type
        abstract def metric
        abstract def values : Array(T)
    end

    class CPU < Sensor(Float64)

        @values = [] of Float64

        getter values : Array(T)
    
        def initialize
            values = [] of Float64
        end
    
        def type
            "CPU"
        end
    
        def metric
            "Temperature"
        end
    
    end
end

