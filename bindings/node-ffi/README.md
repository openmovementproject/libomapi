# libomapi

## Use

```
npm install
npm compile && node index
```


```
node-gyp configure    # --msvs_version=2015
node-gyp build
```

```
link /dump /all build\Release\libomapi.node | findstr Om
```

## Notes on initial creation

```
npm install -g node-gyp
mkdir libomapi
cd libomapi
# create binding.gyp
```
