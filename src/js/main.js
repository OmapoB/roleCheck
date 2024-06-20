const addon = require('bindings')('roleCheck')
const http = require('http')
const fs = require('fs')

const PORT = 4000
const roleMap = {
    0: "гость",
    1: "пользователь",
    2: "администратор",
    3: "не найден"
}

const server = http.createServer(async (req, res) => {
    res.writeHead(200, {
        "Content-type": "text/html; charset=UTF-8",
    })

    if (req.url === "/") {
        fs.readFile("src/main.html", (err, data) => {
            if (err) {
                res.statusCode = 404
                res.end("Page not found")
            } else {
                res.end(data)
            }
        })
    }

    if (req.url === "/role") {
        let body = ""
        for await (const chunk of req) {
            body += chunk
        }

        const userName = body.split("&")[0]
            .split("=")[1]
        const role = roleMap[addon.roleCheck(userName)]
        let message;
        if (role === roleMap[3]) {
            message = `Пользователь ${userName} ${role}`
        } else {
            message = `Пользователь ${userName} имеет привилегию ${role}`
        }

        fs.readFile("src/html/role.html", (err, data) => {
            if (err) {
                res.statusCode = 404
                res.end("Page not found")
            } else {
                page = data.toString().replace(/{message}/g, message);
                res.end(page)
            }
        })
    }

});

server.listen(PORT, () => console.log(`Сервер запущен на порту ${PORT}`))