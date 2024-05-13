// Copyright (c) 2021 Nineva Studios

#include "MGProductsRequestDelegate.h"

@implementation MGProductsRequestDelegate

- (void)productsRequest:(SKProductsRequest *)request didReceiveResponse:(SKProductsResponse *)response {
	if (_onResponseReceived == nil) {
		return;
	}
	
	_onResponseReceived(response);
}

- (void)request:(SKRequest *)request didFailWithError:(NSError *)error {
	if (_onError == nil) {
		return;
	}
	
	_onError(error);
}

@end
