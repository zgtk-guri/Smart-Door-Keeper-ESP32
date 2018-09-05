#ifndef HTML_FILES_H
#define HTML_FILES_H

const char* index_html = "<!DOCTYPE html>\n\
<html>\n\
    <head>\n\
        <meta charset=\"UTF-8\">\n\
        <title>Smart Door Keyper</title>\n\
        <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n\
        <link href=\"https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0/css/bootstrap.min.css\" rel=\"stylesheet\" integrity=\"sha384-Gn5384xqQ1aoWXA+058RXPxPg6fy4IWvTNh0E263XmFcJlSAwiGgFAW/dAiS6JXm\" crossorigin=\"anonymous\">\n\
        <link href=\"https://maxcdn.bootstrapcdn.com/bootswatch/4.0.0/materia/bootstrap.min.css\" rel=\"stylesheet\" integrity=\"sha384-kpxyUW5uLgBOlhLqm8Hga/aWgf7FgZWLhphVqsMgaW0s+G6KF1RZpUb6ygQL3WUE\" crossorigin=\"anonymous\">\n\
    </head>\n\
    <body>\n\
        <div class=\"container\">\n\
            <div class=\"row\">\n\
                <div class=\"page-header\">\n\
                    <h1 style=\"padding: 20px 10px;\">Smart Door Keyper</h1>\n\
                </div>\n\
            </div>\n\
            <div class=\"row\">\n\
                <div class=\"col-9\">\n\
                    <p>現在の状態: <span id=\"status_label\">取得中</span></p>\n\
                </div>\n\
                <div class=\"col-1\">\n\
                    <button class=\"btn\" id=\"reload_button\">更新</button>\n\
                </div>\n\
            </div>\n\
            <div class=\"row\">\n\
                <div class=\"page-header\">\n\
                    <h2>操作</h2>\n\
                </div>\n\
            </div>\n\
            <div class=\"row\">\n\
                <div class=\"col\">\n\
                    <button class=\"btn btn-block btn-success\" id=\"open_button\">解錠</button>\n\
                </div>\n\
                <div class=\"col\">\n\
                    <button class=\"btn btn-block btn-danger\" id=\"close_button\">施錠</button>\n\
                </div>\n\
            </div>\n\
            <div class=\"row\">\n\
                <div class=\"page-header\">\n\
                    <a href=\"/setting\" style=\"padding: 0 10px;\"><h3>設定ページへ</h3></a>\n\
                </div>\n\
                <p>IPアドレス: <span id=\"ip_label\">取得中</span></p>\n\
            </div>\n\
        </div>\n\
        <script src=\"https://ajax.googleapis.com/ajax/libs/jquery/1.11.1/jquery.min.js\"></script>\n\
        <script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0/js/bootstrap.min.js\" integrity=\"sha384-JZR6Spejh4U02d8jOt6vLEHfe/JQGiRRSQQxSfFWpi1MquVdAyjUar5+76PVCmYl\" crossorigin=\"anonymous\"></script>\n\
        <script>\n\
            function reload(){\n\
                $.ajax({url:'/status', method:'GET'})\n\
                    .done((data)=>{\n\
                        $(\"#status_label\").text(data);\n\
                    });\n\
            }\n\
            $(()=>{\n\
                reload();\n\
                $.ajax({url:'/ip_addr', method:'GET'})\n\
                    .done((data)=>{\n\
                        $(\"#ip_label\").text(data);\n\
                    });\n\
                $(\"#reload_button\").click(()=>{\n\
                    reload();\n\
                });\n\
                $(\"#open_button\").click(()=>{\n\
                    $.ajax({url:'/open', method:'GET'})\n\
                        .done((data)=>{\n\
                            relaod();\n\
                        });\n\
                });\n\
                $(\"#close_button\").click(()=>{\n\
                    $.ajax({url:'/close', method:'GET'})\n\
                        .done((data)=>{\n\
                            relaod();\n\
                        });\n\
                })\n\
            });\n\
        </script>\n\
    </body>\n\
</html>";

const char* setting_html = "<!DOCTYPE html>\n\
<html>\n\
    <head>\n\
        <meta charset=\"UTF-8\">\n\
        <title>Smart Door Keyper 設定</title>\n\
        <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n\
        <link href=\"https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0/css/bootstrap.min.css\" rel=\"stylesheet\" integrity=\"sha384-Gn5384xqQ1aoWXA+058RXPxPg6fy4IWvTNh0E263XmFcJlSAwiGgFAW/dAiS6JXm\" crossorigin=\"anonymous\">\n\
        <link href=\"https://maxcdn.bootstrapcdn.com/bootswatch/4.0.0/materia/bootstrap.min.css\" rel=\"stylesheet\" integrity=\"sha384-kpxyUW5uLgBOlhLqm8Hga/aWgf7FgZWLhphVqsMgaW0s+G6KF1RZpUb6ygQL3WUE\" crossorigin=\"anonymous\">\n\
    </head>\n\
    <body>\n\
        <div class=\"container\">\n\
            <div class=\"row\">\n\
                <div class=\"page-header\">\n\
                    <h1 style=\"padding: 20px 10px;\">Smart Door Keyper 初期設定</h1>\n\
                </div>\n\
            </div>\n\
            <form action=\"/setting/set\" method=\"POST\">\n\
                <div class=\"form-group\"><label for=\"wifissid\">Wi-Fi SSID</label><input type=\"text\" name=\"wifissid\" id=\"wifissid\" class=\"form-control\" placeholder=\"Wi-Fi SSID\"></div>\n\
                <div class=\"form-group\"><label for=\"wifipasswd\">Wi-Fi Passphrase</label><input type=\"password\" name=\"wifipasswd\" id=\"wifipasswd\" class=\"form-control\" placeholder=\"Wi-Fi Passphrase\"></div>\n\
                <hr>\n\
                <div class=\"form-group\"><label for=\"mainpasswd\">Setting Password</label><input type=\"password\" name=\"settingpasswd\" id=\"settingpasswd\" class=\"form-control\" placeholder=\"Setting Password\"></div>\n\
                <div class=\"row\"><input class=\"btn btn-primary\" type=\"submit\" value=\"送信\"></div>\n\
            </form>\n\
            <div class=\"row\">\n\
                <div class=\"page-header\">\n\
                    <h2 style=\"padding: 15px 10px;\">BLE用認証コード</h2>\n\
                </div>\n\
            </div>\n\
            <div class=\"row\">\n\
                <p id=\"ble_key\">please wait...</p>\n\
            </div>\n\
        </div>\n\
        <script src=\"https://ajax.googleapis.com/ajax/libs/jquery/1.11.1/jquery.min.js\"></script>\n\
        <script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0/js/bootstrap.min.js\" integrity=\"sha384-JZR6Spejh4U02d8jOt6vLEHfe/JQGiRRSQQxSfFWpi1MquVdAyjUar5+76PVCmYl\" crossorigin=\"anonymous\"></script>\n\
        <script>\n\
            $(function(){\n\
                $.ajax({url:'/setting/key', method:'GET'})\n\
                    .done((data)=>{\n\
                        $('#ble_key').text(data);\n\
                    });\n\
            });\n\
        </script>\n\
    </body>\n\
</html>";

#endif