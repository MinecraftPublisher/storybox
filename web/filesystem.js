function parse_fs(element) {
    const filesystem = {}
    for(let e of element.qs('file')) filesystem[e.attr('name')] = e.innerHTML
    for(let e of element.qs('directory')) filesystem[e.attr('name')] = parse_fs(e)

    return filesystem
}

if(document.querySelector('files')) {
    const files_web = document.querySelector('files')
    globalThis.filesystem = parse_fs(files_web)
}

function load_file(path) {
    localStorage.setItem('location', path)
    let file = globalThis.filesystem
    let name = path.split('/')

    for(let i = 0; i < name.length; i++) file = file[name]
    document.querySelector('player').innerHTML = file
}

console.log('[Loaded] filesystem.js')
globalThis.load_file = load_file

let location = localStorage.getItem('location')
if(location) load_file(location)
else load_file('main')
