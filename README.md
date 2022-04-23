# crsensor

WIP sample to use c bindings with crystal lang

## Development

First compile the clib file because that depends on the macos
```
    cd clib 
    make clean
    make
    cp binsensor.o ../src/ 
    cd ..
    shards run 
     Dependencies are satisfied
     Building: crsensor
     Executing: crsensor 
     49.875
```

## Contributing

1. Fork it (<https://github.com/leofigy/crsensor/fork>)
2. Create your feature branch (`git checkout -b my-new-feature`)
3. Commit your changes (`git commit -am 'Add some feature'`)
4. Push to the branch (`git push origin my-new-feature`)
5. Create a new Pull Request

## Contributors

- [leofigy](https://github.com/leofigy) - creator and maintainer
