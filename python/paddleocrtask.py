# PIL（Python Imaging Library）已经停止维护，其官方维护者推荐使用Pillow作为PIL包的替代方案
# pip install Pillow
from paddleocr import PaddleOCR, draw_ocr

def paddleocr_execute(img_bytes, width, height):
    # Paddleocr目前支持的多语言语种可以通过修改lang参数进行切换
    # 例如`ch`, `en`, `fr`, `german`, `korean`, `japan`
    ocr = PaddleOCR(use_angle_cls=True, lang="ch")
    # img_path = 'D:/User/Qt/QtProject/images/list_menu.png'
    # 'D:/User/Qt/QtProject/images/lay_word.png'
    result = ocr.ocr(img_bytes, cls=True)
    result = result[0]
    return result

