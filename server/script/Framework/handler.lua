local imported = {}
function import(file)
	imported[file] = true
	return require(file)
end
function reimport(file)
	_G.package.loaded[file]  = nil
	imported[file] = true
    return require(file)
end
function clearimported()
	for file, _ in pairs(imported) do
		_G.package.loaded[file]  = nil
		imported[file] = nil
	end
end
