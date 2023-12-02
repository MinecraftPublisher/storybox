function TYPE_PARSE() {
    let elements = document.querySelectorAll('player type[content][played="0"]')
    for(let el of elements) {
        let child = el.querySelector('text')
        let text = el.getAttribute('content').replaceAll('\n', '<br>')
        let buttons = el.querySelectorAll('buttons button.jump')
        let length = text.length
        let i = 0

        for(let jump of buttons) jump.onclick = () => setTimeout(() => load_file(jump.attr('target') ?? 'not_found'), 750)

        el.setAttribute('played', '0.5')

        el.onclick = () => { i += 15; if(i > length) i = length }
        document.body.onkeypress = () => { i += 15; if(i > length) i = length }

        let S = setInterval(() => {
            if(i < length) {
                child.innerHTML = text.substring(0, i) + '<cursor></cursor>'
                i++
                return
            }

            clearInterval(S)
            child.innerHTML = text + '<cursor></cursor>'
            el.setAttribute('played', '1')
            el.onclick = () => {}
            document.body.onkeypress = () => {}
        }, 45)
    }
}


setTimeout(() => {
    console.log('[Loaded] Typewriter.js')
    let TYPE_READER = setInterval(TYPE_PARSE, 50)
    TYPE_PARSE()
}, 500)
