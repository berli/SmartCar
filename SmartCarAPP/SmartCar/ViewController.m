//
//  ViewController.m
//  SmartCar
//
//  Created by TianYuan on 2018/1/26.
//  Copyright © 2018年 SmartCar. All rights reserved.
//

#import "ViewController.h"
#import "GCDAsyncSocket.h"

@interface ViewController ()

@property (strong, nonatomic) IBOutlet UITextView *textView;
@property (strong, nonatomic) GCDAsyncSocket *socket;
@property (assign, nonatomic) uint8_t *array;
@property (weak, nonatomic) IBOutlet UITextField *serverIP;

@property (weak, nonatomic) IBOutlet UITextField *serverPort;


@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    [self connectAction];
    // Do any additional setup after loading the view, typically from a nib.
}

- (void)textViewAddText:(NSString *)text {
    //加上换行
    self.textView.text = [text stringByAppendingFormat:@"\n%@",_textView.text ];
}

- (void)disconnectAction {
    
    [_socket disconnect];
    
}

- (void)connectAction{
    
    if (![_socket isConnected]) {
        
        self.socket = [[GCDAsyncSocket alloc]initWithDelegate:self delegateQueue:dispatch_get_main_queue()];
        //socket.delegate = self;
        NSError *err = nil;
        if(![_socket connectToHost:self.serverIP.text onPort:[self.serverPort.text intValue] error:&err])
        {
            [self textViewAddText:err.description];
        }else
        {
            [self textViewAddText:@"connect ..."];
        }
   
    }else{
        [self textViewAddText:@"has connected"];
    }
    
}
-(void)socket:(GCDAsyncSocket *)sock didConnectToHost:(NSString *)host port:(uint16_t)port
{
    [self textViewAddText:[NSString stringWithFormat:@"connect to  %@",host]];
    [_socket readDataWithTimeout:-1 tag:0];
}
-(void)socket:(GCDAsyncSocket *)sock didReadData:(NSData *)data withTag:(long)tag
{
    
//    Byte *resverByte = (Byte *)[data bytes];
    NSString * newMessage = [self hexStringFromData:data];
    [self showData: newMessage];
    [_socket readDataWithTimeout:-1 tag:0];
}

- (void)socketDidDisconnect:(GCDAsyncSocket *)sock withError:(NSError *)err
{
    _socket = nil;
    [self showData: @"connect error"];
    return ;
}

-(void)setDataWithID:(Byte)ID and:(Byte)OperatID{
    if ([_socket isConnected] ) {
        unsigned char carData[7] = {0x7E,ID,0x01,0x00,OperatID,0x00,0x7E};
        
        unsigned char  CRCSum=carData[1]+carData[2]+carData[3]+carData[4]-0x01;
        
        carData[5]= CRCSum;
        
        
        NSData *d1 = [NSData dataWithBytes:carData length:sizeof(carData)];
        
        NSString *str =  [self hexStringFromData:d1];
        
        [_socket writeData:d1 withTimeout:-1 tag:0];
        
        [self showData: str];
    } else {
        [self showData: @"connect close"];
    }
  

}


-(void)showData:(NSString *)newMessage{
    
    NSDateFormatter  *dateformatter=[[NSDateFormatter alloc] init];
    [dateformatter setDateFormat:@"YYYY-MM-dd HH:mm:ss.SSS"];
    NSString *locationString=[dateformatter stringFromDate:[NSDate date]];
    
    [self textViewAddText:[NSString stringWithFormat:@"%@  %@",locationString, newMessage]];
    
    [_socket readDataWithTimeout:-1 tag:0];

}

- (NSString *)hexStringFromData:(NSData *)data
{
    NSAssert(data.length > 0, @"data.length <= 0");
    NSMutableString *hexString = [[NSMutableString alloc] init];
    const Byte *bytes = data.bytes;
    for (NSUInteger i=0; i<data.length; i++) {
        Byte value = bytes[i];
        Byte high = (value & 0xf0) >> 4;
        Byte low = value & 0xf;
        [hexString appendFormat:@"%x%x", high, low];
    }//for
    return hexString;
}

-(NSData *)dataFromHexString:(NSString *)hexString
{
    NSAssert((hexString.length > 0) && (hexString.length % 2 == 0), @"hexString.length mod 2 != 0");
    NSMutableData *data = [[NSMutableData alloc] init];
    for (NSUInteger i=0; i<hexString.length; i+=2) {
        NSRange tempRange = NSMakeRange(i, 2);
        NSString *tempStr = [hexString substringWithRange:tempRange];
        NSScanner *scanner = [NSScanner scannerWithString:tempStr];
        unsigned int tempIntValue;
        [scanner scanHexInt:&tempIntValue];
        [data appendBytes:&tempIntValue length:1];
    }
    return data;
}

- (NSData *)dataWithReverse:(NSData *)srcData
{

    NSUInteger byteCount = srcData.length;
    NSMutableData *dstData = [[NSMutableData alloc] initWithData:srcData];
    NSUInteger halfLength = byteCount / 2;
    for (NSUInteger i=0; i<halfLength; i++) {
        NSRange begin = NSMakeRange(i, 1);
        NSRange end = NSMakeRange(byteCount - i - 1, 1);
        NSData *beginData = [srcData subdataWithRange:begin];
        NSData *endData = [srcData subdataWithRange:end];
        [dstData replaceBytesInRange:begin withBytes:endData.bytes];
        [dstData replaceBytesInRange:end withBytes:beginData.bytes];
    }//for
    
    return dstData;
}

- (NSData *)byteFromUInt8:(uint8_t)val
{
    NSMutableData *valData = [[NSMutableData alloc] init];
    
    unsigned char valChar[1];
    valChar[0] = 0xff & val;
    [valData appendBytes:valChar length:1];
    
    return [self dataWithReverse:valData];
}

- (IBAction)buttonClickedAction:(UIButton *)sender {
    NSInteger sendertag = sender.tag;
    
    switch (sendertag) {
            
        case 2:
            [self  connectAction];
            break;
        case 3:
            [self  disconnectAction];
            break;
        case 4:
            [self setDataWithID:0x01 and:0x01];//开启
            break;
        case 5:
            [self setDataWithID:0x01 and:0x02];//关闭
            break;
        case 6:
            [self setDataWithID:0x01 and:0x03];//上下左右停
            break;
        case 7:
            [self setDataWithID:0x01 and:0x04];
            break;
        case 8:
            [self setDataWithID:0x01 and:0x05];
            break;
        case 9:
            [self setDataWithID:0x03 and:0x02];//灯光
            break;
        case 10:
            [self setDataWithID:0x02 and:0x02];//蜂鸣器
            break;
            
        default:
            
            break;
    }
    

}


- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}


@end
