# CVE finder:

This was done just because I was really really really lazy to read the news for cves to maintain my computer

Sooo the best way is to use a discord bot to do the job for me right :D

This bot searches through the cve list by sending requests to NVD api based on publish date every hour using libcurl and sends a message into discord to inform me :D

Stonks ngl AHAHHA


Discord bot is done using sleepy discord library made by yourWaifu which is a c++ library for making discord bots

Chose this library to actually learn some c++ programming because c++ is something I always wanted to learn but I have like 0 motivation and brain cells to learn. So with this cool oppurtunity of a cool break why not right :D

## Features:
* Echoes messages
* Plays scissors paper stone with you
* Sends new cves every hour

## Libraries:
* sleepy-discord:
  * https://github.com/yourWaifu/sleepy-discord
* nlohmann json:
  * https://github.com/nlohmann/json
* fmt:
  * https://github.com/fmtlib/fmt