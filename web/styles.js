const sass = await import('https://jspm.dev/sass')
const data = await fetch('styles.scss').then(e => e.text())

document.querySelector('style.scss').innerHTML = sass.compileString(data).css
document.body.setAttribute('loaded', '')

console.log('[Loaded] Styles.js')
