import smtplib 
from email import encoders
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart
from email.mime.base import MIMEBase
from bs4 import BeautifulSoup as bs


def send_mail(Mail):
    server = smtplib.SMTP("smtp.gmail.com", 587)
    server.starttls()
    server.login("Krishna.p.2020.2002", "DfiRBVcaHQYj6Z5")
    server.sendmail(Mail["From"], Mail["TO"], Mail.as_string())
    print("mail sent...")
    server.quit()
   

def Send_Mail(TO, html):
    Mail = MIMEMultipart("alternative")
    Mail["From"] = "Krishna.p.2020.2002@gmail.com"
    Mail["To"] = TO
    Mail["Subject"] = "Accident notification IOT application" 
    text = bs(html, "html.parser").text
    text_part = MIMEText(text, "plain")
    html_part = MIMEText(html, "html")
    Mail.attach(text_part)
    Mail.attach(html_part)
    print("sending mail to  ...",TO)
    send_mail(Mail)
    print("done mailing...")

#inlcude hospital mail
#interface to accecpt location not body.
