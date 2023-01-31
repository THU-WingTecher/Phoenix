// Code generated by mockery v1.0.0. DO NOT EDIT.

package mocks

import (
	comm "github.com/hyperledger/fabric/core/comm"
	common "github.com/hyperledger/fabric/protos/common"

	mock "github.com/stretchr/testify/mock"

	orderer "github.com/hyperledger/fabric/protos/orderer"
)

// HeaderStreamClient is an autogenerated mock type for the HeaderStreamClient type
type HeaderStreamClient struct {
	mock.Mock
}

// Close provides a mock function with given fields:
func (_m *HeaderStreamClient) Close() {
	_m.Called()
}

// Disconnect provides a mock function with given fields:
func (_m *HeaderStreamClient) Disconnect() {
	_m.Called()
}

// GetEndpoint provides a mock function with given fields:
func (_m *HeaderStreamClient) GetEndpoint() string {
	ret := _m.Called()

	var r0 string
	if rf, ok := ret.Get(0).(func() string); ok {
		r0 = rf()
	} else {
		r0 = ret.Get(0).(string)
	}

	return r0
}

// Recv provides a mock function with given fields:
func (_m *HeaderStreamClient) Recv() (*orderer.DeliverResponse, error) {
	ret := _m.Called()

	var r0 *orderer.DeliverResponse
	if rf, ok := ret.Get(0).(func() *orderer.DeliverResponse); ok {
		r0 = rf()
	} else {
		if ret.Get(0) != nil {
			r0 = ret.Get(0).(*orderer.DeliverResponse)
		}
	}

	var r1 error
	if rf, ok := ret.Get(1).(func() error); ok {
		r1 = rf()
	} else {
		r1 = ret.Error(1)
	}

	return r0, r1
}

// Send provides a mock function with given fields: _a0
func (_m *HeaderStreamClient) Send(_a0 *common.Envelope) error {
	ret := _m.Called(_a0)

	var r0 error
	if rf, ok := ret.Get(0).(func(*common.Envelope) error); ok {
		r0 = rf(_a0)
	} else {
		r0 = ret.Error(0)
	}

	return r0
}

// UpdateEndpoints provides a mock function with given fields: endpoints
func (_m *HeaderStreamClient) UpdateEndpoints(endpoints []comm.EndpointCriteria) {
	_m.Called(endpoints)
}

// UpdateReceived provides a mock function with given fields: blockNumber
func (_m *HeaderStreamClient) UpdateReceived(blockNumber uint64) {
	_m.Called(blockNumber)
}